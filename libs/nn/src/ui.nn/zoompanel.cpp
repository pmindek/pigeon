#include "zoompanel.h"
#include <QtDebug>

ZoomPanel::ZoomPanel(QWidget *parent) :
    QWidget(parent)
{
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	this->darkColor = QColor(0,0,0,255);
	this->lightColor = QColor(128, 128, 128, 255);

	this->sliderWidth = this->width();
	this->sliderFrom = 0.0;
	this->sliderTo = 1.0;
	this->setFixedHeight(15);
	this->dragging = DRAG_NOTHING;
}

void ZoomPanel::setColors(QColor darkColor, QColor lightColor)
{
	this->darkColor = darkColor;
	this->lightColor = lightColor;
}

void ZoomPanel::fixRange()
{
	if (this->sliderFrom > this->sliderTo)
	{
		qreal temp = this->sliderTo;
		this->sliderTo = this->sliderFrom;
		this->sliderFrom = temp;
	}

	if (this->sliderFrom == this->sliderTo)
	{
		if (this->sliderFrom < 0.5)
		{
			this->sliderTo += 0.01;
		}
		else
		{
			this->sliderFrom -= 0.01;
		}
	}
}

void ZoomPanel::setFrom(qreal from)
{
	this->sliderFrom = qMin(1.0, qMax(0.0, from));
	this->fixRange();
	emit rangeChanged(this->sliderFrom, this->sliderTo);
}

void ZoomPanel::setTo(qreal to)
{
	this->sliderTo = qMin(1.0, qMax(0.0, to));
	this->fixRange();
	emit rangeChanged(this->sliderFrom, this->sliderTo);
}

void ZoomPanel::paintEvent(QPaintEvent *e)
{
	QPainter p(this);

	p.fillRect(0, 0, this->sliderWidth, this->height(), lightColor);

	int x0 = (int) ((qreal) this->sliderWidth * this->sliderFrom);
	int x1 = (int) ((qreal) this->sliderWidth * this->sliderTo);

	p.fillRect(x0, 0, x1 - x0, this->height(), darkColor);
}

void ZoomPanel::mouseDoubleClickEvent(QMouseEvent *e)
{
	this->sliderFrom = 0.0;
	this->sliderTo = 1.0;
	emit rangeChanged(this->sliderFrom, this->sliderTo);
	this->update();
}

void ZoomPanel::mousePressEvent(QMouseEvent *e)
{
	this->dragFrom = e->pos();

	if (e->button() == Qt::LeftButton)
	{
		dragging = DRAG_FROM;
		this->setFrom((qreal) e->pos().x() / (qreal) this->sliderWidth);
	}

	if (e->button() == Qt::RightButton)
	{
		dragging = DRAG_TO;
		this->setTo((qreal) e->pos().x() / (qreal) this->sliderWidth);
	}

	if (e->button() == Qt::MiddleButton)
	{
		dragging = DRAG_RANGE;
	}

	this->update();
}

void ZoomPanel::mouseMoveEvent(QMouseEvent *e)
{
	if (dragging == DRAG_FROM)
	{
		this->setFrom((qreal) e->pos().x() / (qreal) this->sliderWidth);
	}
	else if (dragging == DRAG_TO)
	{
		this->setTo((qreal) e->pos().x() / (qreal) this->sliderWidth);
	}
	else if (dragging == DRAG_RANGE)
	{
		int delta = e->pos().x() - dragFrom.x();

		if (delta < 0 && -delta > this->sliderFrom * this->sliderWidth)
		{
			delta = -this->sliderFrom * this->sliderWidth;
		}
		if (delta > 0 && delta > this->sliderWidth - this->sliderTo * this->sliderWidth)
		{
			delta = this->sliderWidth - this->sliderTo * this->sliderWidth;
		}

		this->sliderFrom += (qreal) (delta) / (qreal) this->sliderWidth;
		this->sliderTo += (qreal) (delta) / (qreal) this->sliderWidth;

		this->dragFrom = e->pos();
		emit rangeChanged(this->sliderFrom, this->sliderTo);
	}

	this->update();
}

void ZoomPanel::mouseReleaseEvent(QMouseEvent *e)
{
	dragging = DRAG_NOTHING;
}

void ZoomPanel::resizeEvent(QResizeEvent *event)
{
	this->setSliderWidth(this->width());
}

void ZoomPanel::setSliderWidth(int w)
{
	this->sliderWidth = w;
	this->update();
}
