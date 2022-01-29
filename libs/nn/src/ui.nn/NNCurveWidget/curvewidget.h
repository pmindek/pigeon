#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QList>
#include <QVector2D>
#include <QResizeEvent>
#include <QSharedMemory>
#include "curve.h"

#include <QtDebug>

#define CURVE_POINT_RADIUS 6
#define CURVE_POINT_REMOVE_RADIUS 10
#define ZOOM_BAR_HEIGHT 15

class CurveWidget : public QWidget
{
Q_OBJECT
public:
	CurveWidget(QWidget *parent = 0);
	~CurveWidget();
//	QSize sizeHint() const;
	void paintEvent(QPaintEvent *event);
	int heightForWidth(int w);

	void setThresholdVisible(bool thresholdVisible);


	QList<Curve *> getCurves();
	QList<qreal> getCurve(int resolution, int which = -1);

	inline int getCurrentCurveIndex()
	{
		return this->currentCurve;
	}

	inline QColor getCurrentCurveColor()
	{
		return this->curves[currentCurve]->getColor();
	}

protected:
	void resizeEvent(QResizeEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	Curve *createCurve();

	QList<Curve*> curves;

	int moving;
	int deleteLine;
	CurvePoint thresholdPoint, minimumThresholdPoint, seekPoint;
	bool thresholdVisible;

	void removeCurrentPoint();
	bool removeDefaultCurve();

	int currentCurve;

	qreal timePos;

	qreal xFrom;
	qreal xTo;

	QSharedMemory sharedPosition;

	void loadSharedPosition();

signals:
	void changed();
	void curveListChanged();
	void thresholdChanged(double threshold);
	void minThresholdChanged(double minThreshold);
	void seeked(qreal);
	void widthChanged(int size);

public slots:
	void setCurrentCurveIndex(int currentCurve);
	void changeCurveType(int curveType);
	void changeCurveColor(QColor color);
	void reset();
	void setTimePos(qreal);
	void setXRange(qreal, qreal);

	bool newCurve();
	bool openCurve();
	bool saveCurve(int index);
	bool saveCurve();
	bool saveCurveAs(int index);
	bool saveCurveAs();
	bool saveAllCurves();
	bool closeCurve(int index, bool allowEmpty = false);
	bool closeCurve();
	bool closeAllCurves();
};

#endif // CURVEWIDGET_H
