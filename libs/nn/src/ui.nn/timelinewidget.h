#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QSharedMemory>
#include "../util/post.h"

class TimelineWidget : public QWidget
{
	Q_OBJECT
public:
	explicit TimelineWidget(Post *post, QWidget *parent = 0);

protected:
	void loadPositionIntoSharedMemory(qreal position);
	void updatePosition(qreal position, qreal relativePosition = 0.0);

	qreal msToPixels(qreal ms, qreal duration, bool offset = true);
	qreal pixelsToMs(qreal pixels, qreal duration);

	void calculateTrackHeights();
	TimelineItem *getItemAt(QPointF cursor);
	TimelineItem *getItemAtPosition(qreal position);

	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void resizeEvent(QResizeEvent *event);

	bool dragging;

	QSharedMemory sharedPosition;

	Post *post;

	qreal position;

	qreal rangeFrom;
	qreal rangeTo;

	int tracks; //number of tracks
	qreal headerHeight; //the height of the header
	qreal separatorHeight; //the height of the separator between tracks
	QList<qreal> trackHeights; //total height of each track
	QList<qreal> lineHeights; //line height for each track

signals:
	void seek(qreal);
	void updated();

public slots:
	void setPosition(qreal ms);
	void setPosition(int ms);
	void setRange(qreal, qreal);
	void updateTimeline();
};

#endif // TIMELINEWIDGET_H
