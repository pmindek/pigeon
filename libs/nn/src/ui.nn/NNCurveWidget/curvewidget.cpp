#include "curvewidget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>

Curve *CurveWidget::createCurve()
{
	Curve *curve = new Curve(this);
	connect(curve, SIGNAL(curveChanged()), this, SIGNAL(curveListChanged()));
	return curve;
}

CurveWidget::CurveWidget(QWidget *parent) : QWidget(parent),
	sharedPosition("nn.curves.shared.position", this)
{
	this->curves << createCurve();
	emit curveListChanged();
	emit changed();

	currentCurve = 0;

	thresholdPoint.setRelativePosition(0.5, 0.5);
	minimumThresholdPoint.setRelativePosition(0.2, 0.2);

	seekPoint.setRelativePosition(0.0, 0.0);

	moving = -1;
	xFrom = 0.0;
	xTo = 1.0;

	this->thresholdVisible = false;

	CurvePoint::setXRange(xFrom, xTo);
}

void CurveWidget::loadSharedPosition()
{
	if (!sharedPosition.attach()) {
		return;
	}

	QBuffer buffer;
	QDataStream in(&buffer);
	qreal position;

	sharedPosition.lock();
	buffer.setData((char*)sharedPosition.constData(), sharedPosition.size());
	buffer.open(QBuffer::ReadOnly);
	in >> position;
	sharedPosition.unlock();

	sharedPosition.detach();

	this->seekPoint.setRelativeX(position);
}

CurveWidget::~CurveWidget()
{

}

/*QSize CurveWidget::sizeHint () const
{
	QSize size;
	size.setWidth(800);
	size.setHeight(800);
	return size;
}*/

void CurveWidget::resizeEvent(QResizeEvent *event)
{
	/*int side = qMin(event->size().width() - 1, event->size().height() - 1);
	CurvePoint::setPlaneSize(side, side);
	*/
	CurvePoint::setPlaneSize(event->size());
	emit widthChanged(event->size().width());
}

bool CurveWidget::removeDefaultCurve()
{
	qDebug() << this->curves[currentCurve]->isSaved() << this->curves[currentCurve]->isModified();
	if (!this->curves[currentCurve]->isSaved() && !this->curves[currentCurve]->isModified())
	{
		qDebug() << "yep";
		/*if (this->curves[0]->isModified())
		{
			int result =
			QMessageBox::question(this,
								  tr("File modified"),
								  tr("The file ") + this->curves[this->currentCurve]->getFileName() + tr(" has been modified. Do you want to save it?"),
								  QMessageBox::Yes | QMessageBox::Default,
								  QMessageBox::No,
								  QMessageBox::Cancel | QMessageBox::Escape
					);

			if (result == QMessageBox::Yes)
			{
				this->saveCurve();
			}
			if (result == QMessageBox::Cancel)
			{
				return false;
			}
		}*/

		this->closeCurve(currentCurve, true);
	}
	return true;
}

bool CurveWidget::newCurve()
{
	/*if (this->curves[currentCurve]->isModified())
	{
		int result =
		QMessageBox::question(this,
							  tr("File modified"),
							  tr("The file ") + this->curves[this->currentCurve]->getFileName() + tr(" has been modified. Do you want to save it?"),
							  QMessageBox::Yes | QMessageBox::Default,
							  QMessageBox::No,
							  QMessageBox::Cancel | QMessageBox::Escape
				);

		if (result == QMessageBox::Yes)
		{
			this->saveCurve();
		}
		if (result == QMessageBox::Cancel)
		{
			return false;
		}
	}*/

	if (!removeDefaultCurve())
	{
		return false;
	}

	this->curves << createCurve();
	this->currentCurve = this->curves.count() - 1;
	this->update();
	emit changed();
	emit curveListChanged();

	return true;
}

bool CurveWidget::openCurve()
{	
	QStringList fileNames =
	QFileDialog::getOpenFileNames(this,
								 tr("Open..."),
								 this->curves[currentCurve]->getFileName(false),
								 tr("JSON files (*.json);; All files (*.*)"));

	if (!fileNames.isEmpty())
	{
		if (!removeDefaultCurve())
		{
			return false;
		}

		bool ok = false;

		foreach (QString fileName, fileNames)
		{
			Curve *curve = createCurve();
			this->curves << curve;
			this->currentCurve = this->curves.count() - 1;
			bool curveOk = this->curves[currentCurve]->load(fileName);;

			if (!curveOk)
			{
				delete this->curves.takeLast();
				this->currentCurve = this->curves.count() - 1;
			}

			ok = ok || curveOk;
		}

		if (ok)
		{
			emit changed();
			emit curveListChanged();
		}

		return ok;
	}
	else
	{
		return false;
	}

	return false;
}

bool CurveWidget::saveCurve(int index)
{
	if (!this->curves[index]->isSaved())
	{
		return this->saveCurveAs(index);
	}
	else
	{
		return this->curves[index]->save();
	}
}

bool CurveWidget::saveCurve()
{
	return this->saveCurve(currentCurve);
}

bool CurveWidget::saveCurveAs(int index)
{
	QString fileName =
	QFileDialog::getSaveFileName(this,
								 tr("Save ") + this->curves[index]->getFileName(true) + tr(" as..."),
								 this->curves[index]->getFileName(false),
								 tr("JSON files (*.json)"));

	if (!fileName.isEmpty())
	{
		bool ok = this->curves[index]->save(fileName);

		if (ok)
		{
			emit changed();
			emit curveListChanged();
		}

		return ok;
	}
	else
	{
		return false;
	}
}

bool CurveWidget::saveCurveAs()
{
	return this->saveCurveAs(currentCurve);
}

bool CurveWidget::saveAllCurves()
{
	bool ok = true;
	for (int i = 0; i < this->curves.count(); i++)
	{
		ok = ok && this->saveCurve(i);
	}

	return ok;
}

bool CurveWidget::closeCurve(int index, bool allowEmpty)
{
	if (index < 0 || index >= this->curves.count())
	{
		return false;
	}

	if (this->curves[index]->isModified())
	{
		int result =
		QMessageBox::question(this,
							  tr("File modified"),
							  tr("The file ") + this->curves[index]->getFileName() + tr(" has been modified. Do you want to save it?"),
							  QMessageBox::Yes | QMessageBox::Default,
							  QMessageBox::No,
							  QMessageBox::Cancel | QMessageBox::Escape
				);

		if (result == QMessageBox::Yes)
		{
			this->saveCurve();
		}
		if (result == QMessageBox::Cancel)
		{
			return false;
		}
	}

	Curve *toClose = this->curves.takeAt(index);
	delete toClose;

	//if we deleted the last curve in the list, the second to last (now the last) curve will be the current one
	if (currentCurve >= this->curves.count())
	{
		currentCurve--;
	}

	//if the current curve is -1, there are no curves left. we create a new one
	if (currentCurve < 0 && !allowEmpty)
	{
		currentCurve = 0;
		this->curves << createCurve();
		emit changed();
	}

	emit curveListChanged();

	return true;
}

bool CurveWidget::closeCurve()
{
	return this->closeCurve(currentCurve);
}

bool CurveWidget::closeAllCurves()
{
	while (this->curves.count() > 0)
	{
		if (!this->closeCurve(0, true))
		{
			return false;
		}
	}
	currentCurve = 0;
	this->curves << createCurve();
	emit changed();
	emit curveListChanged();

	return true;
}

void CurveWidget::setCurrentCurveIndex(int currentCurve)
{
	if (currentCurve >= 0 && currentCurve < this->curves.count())
	{
		this->currentCurve = currentCurve;

		this->update();
		emit changed();
	}
}

void CurveWidget::changeCurveType(int curveType)
{
	this->curves[currentCurve]->setCurveType(curveType);

	this->update();
	emit changed();
}

void CurveWidget::changeCurveColor(QColor color)
{
	this->curves[currentCurve]->setColor(color);

	this->update();
	emit changed();
}

void CurveWidget::reset()
{
	this->curves[currentCurve]->reset();

	this->update();
	emit changed();
}

void CurveWidget::setTimePos(qreal timePos)
{
	this->timePos = timePos;

	update();
}

void CurveWidget::setXRange(qreal from, qreal to)
{
	this->xFrom = from;
	this->xTo = to;

	CurvePoint::setXRange(from, to);

	update();
}

void CurveWidget::setThresholdVisible(bool thresholdVisible)
{
	this->thresholdVisible = thresholdVisible;
	this->update();
	emit changed();
}

QList<Curve *> CurveWidget::getCurves()
{
	return this->curves;
}

QList<qreal> CurveWidget::getCurve(int resolution, int which)
{
	return this->curves[which]->getCurve(resolution);
}

void CurveWidget::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
	QPainter painter(this);

	//int side = qMin(this->width() - 1, this->height() - 1);

	QPen pen;


	painter.setClipRect(0, 0, this->width() - 1, this->height() - 1);

	painter.setBrush(QBrush(QColor(0, 0, 0, 255)));
	pen.setColor(QColor(100, 100, 100, 255));
	pen.setStyle(Qt::SolidLine);
	painter.setPen(pen);
	painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

	pen.setStyle(Qt::DotLine);
	painter.setPen(pen);

	double widthF = (double) this->width();
	double heightF = (double) this->height();

	int sectionsX = 4 * 2;
	double alphaX = qMin(1.0, qMax(0.0, (double) this->width() / 1024.0));
	for (int i = 1; i < sectionsX; i++)
	{
		double rangedX = widthF * ((((double) i / sectionsX) - this->xFrom) / (this->xTo - this->xFrom));

		if (i % 2 == 0)
		{
			pen.setColor(QColor(100, 100, 100, 255));
		}
		else
		{
			pen.setColor(QColor(100, 100, 100, (int) (128.0 * alphaX)));
		}
		painter.setPen(pen);

		painter.drawLine((int) rangedX, 0, (int) rangedX, this->height() - 1);
	}

	int sectionsY = 4 * 2;
	double alphaY = qMin(1.0, qMax(0.0, (double) this->height() / 1024.0));
	for (int i = 1.0; i < sectionsY; i++)
	{
		if (i % 2 == 0)
		{
			pen.setColor(QColor(100, 100, 100, 255));
		}
		else
		{
			pen.setColor(QColor(100, 100, 100, (int) (128.0 * alphaY)));
		}
		painter.setPen(pen);

		painter.drawLine(0, (int) (heightF * ((double) i / sectionsY)), this->width() - 1, (int) (heightF * ((double) i / sectionsY)));
	}

	painter.setRenderHint(QPainter::Antialiasing);

	pen.setColor(this->curves[currentCurve]->getColor());
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(2);
	painter.setPen(pen);


	if (this->curves[currentCurve]->getControlPoints()->length() > 0)
	{
		//painter.drawLine(0, this->curves[currentCurve]->getControlPoints()->first().getAbsoluteY(), this->curves[currentCurve]->getControlPoints()->first().getAbsoluteX(), this->curves[currentCurve]->getControlPoints()->first().getAbsoluteY());
		//painter.drawLine(this->width(), this->curves[currentCurve]->getControlPoints()->last().getAbsoluteY(), this->curves[currentCurve]->getControlPoints()->last().getAbsoluteX(), this->curves[currentCurve]->getControlPoints()->last().getAbsoluteY());






		QVector<QPoint> polygonPoints;
		QVector<QPoint> currentPolygonPoints;
		for (int j = 0; j < this->curves.count(); j++)
		{
			this->curves[j]->precalc();
			polygonPoints.clear();
			for (int i = 0; i < this->width() - 1; i += 4)
			{
				(j == currentCurve ? currentPolygonPoints : polygonPoints) << QPoint(i, (int) this->curves[j]->interpolate((double) i, heightF));
			}
			(j == currentCurve ? currentPolygonPoints : polygonPoints) << QPoint(widthF, (int) this->curves[j]->interpolate(widthF, heightF));

			if (j != currentCurve)
			{
				pen.setWidth(1);
				pen.setColor(QColor(this->curves[j]->getColor().red(), this->curves[j]->getColor().green(), this->curves[j]->getColor().blue(), 100));
				painter.setPen(pen);
				painter.drawPolyline(QPolygon(polygonPoints));
			}
		}
		pen.setWidth(2);
		pen.setColor(this->curves[currentCurve]->getColor());
		painter.setPen(pen);
		painter.drawPolyline(QPolygon(currentPolygonPoints));


		pen.setWidth(2);
		pen.setColor(QColor(255, 255, 255, 255));
		painter.setPen(pen);
		painter.setBrush(QBrush(QColor(64, 64, 64, 255)));

		if (this->curves[currentCurve]->getDrawControlPoints())
		{
			for (int i = 0; i < this->curves[currentCurve]->getControlPoints()->length(); i++)
			{
				painter.drawEllipse(this->curves[currentCurve]->getControlPoints()->at(i).getAbsolutePos(), 5, 5);
			}
		}


		//seek point
		{
			pen.setWidth(2);
			pen.setColor(QColor(64, 192, 255, 128));
			painter.setPen(pen);
			painter.drawLine(seekPoint.getAbsoluteX() + 1, 0.0, seekPoint.getAbsoluteX() + 1, heightF);
		}


		if (thresholdVisible)
		{
			pen.setWidth(2);
			pen.setColor(QColor(255, 255, 255, 128));
			painter.setPen(pen);
			painter.drawLine(0, thresholdPoint.getAbsoluteY() + 1, widthF, thresholdPoint.getAbsoluteY() + 1);

			QLinearGradient grad(QPointF(0, thresholdPoint.getAbsoluteY()), QPointF(0, heightF));
			grad.setColorAt(0, QColor(64, 64, 64, 200));
			grad.setColorAt(1, QColor(64, 64, 64, 0));
			painter.fillRect(0, thresholdPoint.getAbsoluteY() + 1, this->width() + 1, this->width() - thresholdPoint.getAbsoluteY() - 1, grad);
		}

		if (deleteLine >= 0)
		{
			pen.setWidth(2);
			pen.setColor(QColor(255, 0, 0, 128));
			painter.setPen(pen);
			painter.drawLine(deleteLine, 0, deleteLine, this->height());
		}
	}

	pen.setWidth(2);
	pen.setColor(QColor(200, 0, 0, 200));
	painter.setPen(pen);
	qreal timeSlider = ((this->timePos - this->xFrom) / (this->xTo - this->xFrom)) * widthF;
	painter.drawLine(timeSlider, 0, timeSlider, heightF);
}

int CurveWidget::heightForWidth(int w)
{
	return w;
}

void CurveWidget::removeCurrentPoint()
{
	if (this->curves[currentCurve]->getControlPoints()->length() <= 1)
		return;

	this->curves[currentCurve]->getControlPoints()->removeAt(moving);

	this->curves[currentCurve]->setModified(true);

	moving = -1;
	deleteLine = -1;
}

void CurveWidget::mousePressEvent(QMouseEvent *event)
{
	this->loadSharedPosition();

	qreal distance, minDistance;

	if (event->button() == Qt::RightButton)
	{
		if (thresholdVisible)
		{
			thresholdPoint.setAbsolutePosition(event->pos().x(), event->pos().y());
			this->update();
			emit thresholdChanged(thresholdPoint.getRelativeY());
			moving = -2;
		}
		else
		{
			//seeking
			seekPoint.setAbsolutePosition(event->pos().x(), event->pos().y());
			this->update();
			emit seeked(seekPoint.getRelativeX());
			moving = -3;
		}
		return;
	}

	if (event->button() == Qt::MiddleButton)
	{
		this->curves[currentCurve]->setDrawControlPoints(!this->curves[currentCurve]->getDrawControlPoints());
		update();
		return;
	}

	moving = -1;
	deleteLine = -1;

	int where = -1;

	minDistance = -1.0;

	for (int i = 0; i < this->curves[currentCurve]->getControlPoints()->length(); i++)
	{
		if (this->curves[currentCurve]->getControlPoints()->at(i).getAbsoluteX() > event->pos().x() && where < 0)
		{
			where = i;
		}

		if (qAbs(this->curves[currentCurve]->getControlPoints()->at(i).getAbsoluteX() - event->pos().x()) < CURVE_POINT_RADIUS)
		{
			distance = QVector2D(this->curves[currentCurve]->getControlPoints()->at(i).getAbsolutePos() - event->pos()).length();

			if (minDistance < 0 || distance < minDistance)
			{
				minDistance = distance;
				moving = i;
			}
		}
	}

	if (moving >= 0)
	{
		if ((event->modifiers() & Qt::AltModifier) != 0)
		{
			removeCurrentPoint();
			this->update();
			emit changed();
		}
		else
		{
			mouseMoveEvent(event);
		}
	}
	else
	{
		if (where < 0)
		{
			where = this->curves[currentCurve]->getControlPoints()->length();
		}

		CurvePoint curvePoint;
		curvePoint.setAbsolutePosition(event->pos().x(), event->pos().y());
		this->curves[currentCurve]->getControlPoints()->insert(where, curvePoint);

		moving = where;
		mouseMoveEvent(event);
	}

	this->curves[currentCurve]->setModified(true);
}

void CurveWidget::mouseReleaseEvent(QMouseEvent *event)
{
	moving = -1;
	deleteLine = -1;
	this->update();
	emit changed();
}

void CurveWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (moving == -2)
	{
		if (thresholdVisible)
		{
			thresholdPoint.setAbsolutePosition(event->pos().x(), event->pos().y());
			this->update();
			emit thresholdChanged(thresholdPoint.getRelativeY());
		}
		return;
	}

	if (moving == -3)
	{
		seekPoint.setAbsolutePosition(event->pos().x(), event->pos().y());
		emit seeked(seekPoint.getRelativeX());
		this->update();
		return;
	}

	if (moving < 0)
	{
		return;
	}

	deleteLine = -1;

	(*this->curves[currentCurve]->getControlPoints())[moving].setAbsolutePosition(event->pos().x(), event->pos().y(),
																				  (event->modifiers() & Qt::ControlModifier) != 0);

	if ((event->modifiers() & Qt::ShiftModifier) != 0)
	{
		this->seekPoint.setAbsolutePosition(event->pos().x(), event->pos().y(),
											(event->modifiers() & Qt::ControlModifier) != 0);
	}

	if (moving >= 1 && this->curves[currentCurve]->getControlPoints()->at(moving).getAbsoluteX() <= this->curves[currentCurve]->getControlPoints()->at(moving - 1).getAbsoluteX())
	{
		if (this->curves[currentCurve]->getControlPoints()->at(moving - 1).getAbsoluteX() - this->curves[currentCurve]->getControlPoints()->at(moving).getAbsoluteX() > CURVE_POINT_REMOVE_RADIUS)
		{
			removeCurrentPoint();
		}
		else
		{
			deleteLine = this->curves[currentCurve]->getControlPoints()->at(moving - 1).getAbsoluteX();
			(*this->curves[currentCurve]->getControlPoints())[moving].setAbsolutePosition(this->curves[currentCurve]->getControlPoints()->at(moving - 1).getAbsoluteX() + 1, event->pos().y());
		}
	}
	else
	if (moving < this->curves[currentCurve]->getControlPoints()->length() - 1 && this->curves[currentCurve]->getControlPoints()->at(moving).getAbsoluteX() >= this->curves[currentCurve]->getControlPoints()->at(moving + 1).getAbsoluteX())
	{
		if (this->curves[currentCurve]->getControlPoints()->at(moving).getAbsoluteX() - this->curves[currentCurve]->getControlPoints()->at(moving + 1).getAbsoluteX() > CURVE_POINT_REMOVE_RADIUS)
		{
			removeCurrentPoint();
		}
		else
		{
			deleteLine = this->curves[currentCurve]->getControlPoints()->at(moving + 1).getAbsoluteX();
			(*this->curves[currentCurve]->getControlPoints())[moving].setAbsolutePosition(this->curves[currentCurve]->getControlPoints()->at(moving + 1).getAbsoluteX() - 1, event->pos().y());
		}
	}

	/*if ((moving >= 1 && this->curves[currentCurve]->getControlPoints()[moving].getAbsoluteX() - this->curves[currentCurve]->getControlPoints()[moving - 1].getAbsoluteX() < CURVE_POINT_REMOVE_RADIUS) ||
		(moving < this->curves[currentCurve]->getControlPoints()->length() - 1 && this->curves[currentCurve]->getControlPoints()[moving + 1].getAbsoluteX() - this->curves[currentCurve]->getControlPoints()[moving].getAbsoluteX() < CURVE_POINT_REMOVE_RADIUS))
	{
		removeCurrentthis->curves[currentCurve]->getControlPoints()();
	}*/

	this->update();
	emit changed();
}
