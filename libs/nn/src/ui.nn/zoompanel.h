#ifndef ZOOMPANEL_H
#define ZOOMPANEL_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>

#define DRAG_NOTHING -1
#define DRAG_FROM 0
#define DRAG_TO 1
#define DRAG_RANGE 2

class ZoomPanel : public QWidget
{
	Q_OBJECT
public:
	explicit ZoomPanel(QWidget *parent = 0);

	void setFrom(qreal from);
	void setTo(qreal to);

	void setColors(QColor darkColor, QColor lightColor);

protected:
	void paintEvent(QPaintEvent *);
	void mouseDoubleClickEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void resizeEvent(QResizeEvent *event);

private:
	void fixRange();

	int sliderWidth;
	qreal sliderFrom;
	qreal sliderTo;

	int dragging;
	QPoint dragFrom;

	QColor darkColor;
	QColor lightColor;

signals:
	void rangeChanged(qreal, qreal);

public slots:
	void setSliderWidth(int);

};

#endif // ZOOMPANEL_H
