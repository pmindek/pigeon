#include "timelinewidget.h"
#include <QBuffer>

TimelineWidget::TimelineWidget(Post *post, QWidget *parent) : QWidget(parent),
	post(post),
	sharedPosition("nn.curves.shared.position", this)
{
	this->dragging = false;

	this->position = 0;

	this->rangeFrom = 0.0;
	this->rangeTo = 1.0;

	this->setMinimumHeight(80);
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	this->calculateTrackHeights();
}

void TimelineWidget::loadPositionIntoSharedMemory(qreal position)
{
	if (sharedPosition.isAttached())
	{
		sharedPosition.detach();
	}

	// load into shared memory
	QBuffer buffer;
	buffer.open(QBuffer::ReadWrite);
	QDataStream out(&buffer);
	out << position;
	int size = buffer.size();

	if (!sharedPosition.create(size))
	{
		qWarning() << "Unable to create shared memory segment.";
		return;
	}
	sharedPosition.lock();
	char *to = (char*) sharedPosition.data();
	const char *from = buffer.data().data();
	memcpy(to, from, qMin(sharedPosition.size(), size));
	sharedPosition.unlock();
}

void TimelineWidget::resizeEvent(QResizeEvent *event)
{
	this->calculateTrackHeights();
}

void TimelineWidget::setPosition(qreal ms)
{
	TimelineItem *item = this->getItemAtPosition(this->msToPixels(ms, this->post->getDuration()));

	qreal relativePosition = 0.0;
	if (item != 0)
	{
		relativePosition = item->getRelativePosition(ms);
	}


	this->updatePosition(ms, relativePosition);
}

void TimelineWidget::setPosition(int ms)
{
	this->setPosition((qreal) ms);
}

void TimelineWidget::updatePosition(qreal position, qreal relativePosition)
{
	//qDebug() << "setting position" << position;

	this->loadPositionIntoSharedMemory(relativePosition);

	this->position = position;
	update();
}

void TimelineWidget::updateTimeline()
{
	this->calculateTrackHeights();
	emit seek(this->position);
	update();
}

void TimelineWidget::setRange(qreal rangeFrom, qreal rangeTo)
{
	this->rangeFrom = rangeFrom;
	this->rangeTo = rangeTo;

	update();
}

qreal TimelineWidget::msToPixels(qreal ms, qreal duration, bool offset)
{
	if (duration * (rangeTo - rangeFrom) <= 0)
	{
		return 0;
	}

	qreal _offset = offset ? (duration * (rangeFrom)) : 0.0;
	return ((ms - _offset) / (duration * (rangeTo - rangeFrom)) * (qreal) (this->width() - 1));
}

qreal TimelineWidget::pixelsToMs(qreal pixels, qreal duration)
{
	return qMax(0.0, qMin(duration, (duration * (rangeFrom + pixels / (qreal) (this->width() - 1) * (rangeTo - rangeFrom)))));
}

void TimelineWidget::mousePressEvent(QMouseEvent *event)
{
	//seeking
	if (event->button() == Qt::LeftButton)
	{
		this->dragging = true;

		qreal ms = pixelsToMs(event->localPos().x(), this->post->getDuration());

		this->setPosition(ms);
		emit seek(ms);
	}
	//turning effects on/off
	else if (event->button() == Qt::RightButton)
	{
		TimelineItem *item = this->getItemAt(event->localPos());

		PostEffect *effect = dynamic_cast<PostEffect *> (item);

		if (effect != 0)
		{
			effect->visible = !effect->visible;
			update();

			emit updated();
		}
	}
}

void TimelineWidget::mouseMoveEvent(QMouseEvent *event)
{
	TimelineItem *item = this->getItemAt(event->localPos());

	if (this->dragging)
	{
		int ms = pixelsToMs(event->pos().x(), this->post->getDuration());

		this->setPosition(ms);
		emit seek(ms);
	}
}

void TimelineWidget::mouseReleaseEvent(QMouseEvent *event)
{
	this->dragging = false;
}

TimelineItem *TimelineWidget::getItemAt(QPointF cursor)
{
	qreal heightSoFar = this->headerHeight + separatorHeight;

	for (int track = 0; track < this->trackHeights.count(); heightSoFar += (trackHeights[track] + separatorHeight), track++)
	{
		if (cursor.y() >= heightSoFar - separatorHeight && cursor.y() < heightSoFar)
		{
			return 0;
		}
		else if (cursor.y() >= heightSoFar && cursor.y() < heightSoFar + trackHeights[track])
		{
			Timeline timeline = this->post->getTimeline(track);

			foreach (TimelineItem *item, timeline)
			{
				qreal duration = this->post->getDuration();
				qreal x = msToPixels(item->from, duration);
				qreal y = heightSoFar + (qreal) item->line * lineHeights[track];
				qreal w = -1.0 + msToPixels(item->to - item->from, duration, false);
				qreal h = lineHeights[track] - 1.0;

				if (cursor.x() >= x && cursor.x() < x + w && cursor.y() >= y && cursor.y() < y + h)
				{
					return item;
				}
			}

			break;
		}
	}

	return 0;
}

TimelineItem *TimelineWidget::getItemAtPosition(qreal position)
{
	for (int track = 0; track < this->trackHeights.count(); track++)
	{
		Timeline timeline = this->post->getTimeline(track);

		foreach (TimelineItem *item, timeline)
		{
			qreal duration = this->post->getDuration();
			qreal x = msToPixels(item->from, duration);
			qreal w = -1.0 + msToPixels(item->to - item->from, duration, false);

			if (position >= x && position < x + w)
			{
				return item;
			}
		}

		break;
	}

	return 0;
}


void TimelineWidget::calculateTrackHeights()
{
	this->trackHeights.clear();
	this->lineHeights.clear();

	headerHeight = 15;
	separatorHeight = 6;

	tracks = this->post->getNumberOfTracks();

	qreal timelineHeightBrutto = (qreal) this->height() - headerHeight;
	qreal timelineHeightNetto = timelineHeightBrutto - separatorHeight * (qreal) (tracks + 1);

	QList<qreal> trackHeightRatios;
	for (int i = 0; i < tracks; i++)
	{
		trackHeightRatios << (i == tracks - 1 ? 0.5 : 1.0);
	}

	//normalize track height ratios
	qreal trackHeightRatiosSum = 0.0;
	for (int i = 0; i < tracks; i++)
	{
		trackHeightRatiosSum += trackHeightRatios[i];
	}
	for (int i = 0; i < tracks; i++)
	{
		trackHeightRatios[i] /= trackHeightRatiosSum;
	}

	for (int i = 0; i < tracks; i++)
	{
		qreal trackHeight = timelineHeightNetto * trackHeightRatios[i];
		trackHeights << trackHeight;
		lineHeights << trackHeight / (qreal) this->post->getMaxOverlays(i);
	}
}

void TimelineWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);

	QFont font;
	font.setPixelSize(7);
	p.setFont(font);

	p.setRenderHint(QPainter::Antialiasing);

	QPen pen(Qt::NoPen);
	p.setPen(pen);
	p.setBrush(QColor(60, 60, 70, 255));
	p.drawRect(0, 0, this->width(), this->height());

	QList<QColor> colors;
	colors << QColor(210, 40, 20, 255);
	colors << QColor(20, 40, 210, 255);
	colors << QColor(40, 140, 20, 255);
	colors << QColor(210, 140, 20, 255);
	colors << QColor(140, 110, 80, 255);


	bool drawText = true;


	qreal duration = this->post->getDuration();

	//draw tick marks
	qreal msSoFar = 0;
	qreal msPerLine = this->post->getSoundtrack()->calculateDuration(1);

	foreach (SoundtrackSection *section, this->post->getSoundtrack()->getSections())
	{
		for (int i = 0; i < section->patterns.count(); i++)
		{
			//draw lines
			qreal linePixels = msToPixels(msPerLine, duration, false);
			if (linePixels > 1)
			{
				qreal fadeIn = qMin(1.0, (linePixels - 1) / 7.0);
				for (int j = 1; j < section->patterns[i]; j++)
				{
					p.setPen(QColor(60 * 3, 60 * 3, 70 * 3, ((j % this->post->getSoundtrack()->getLpb()) == 0 ? 80 : 25) * fadeIn));
					qreal ms = (qreal) msSoFar + (qreal) j * msPerLine;
					p.drawLine(msToPixels(ms, duration),
							   0.0,
							   msToPixels(ms, duration),
							   (qreal) this->height());
				}
			}

			msSoFar += section->patternDurations[i];
			if (i < section->patterns.count() - 1 || section != this->post->getSoundtrack()->getSections().last())
			{
				p.setPen(QColor(60 * 3, 60 * 3, 70 * 3, 170));
				p.drawLine(msToPixels(msSoFar, duration),
						   0.0,
						   msToPixels(msSoFar, duration),
						   (qreal) this->height());
			}
		}
	}

	//draw tracks
	{
		qreal heightSoFar = headerHeight;

		p.setBrush(QColor(0,0,0,64));
		p.setPen(Qt::NoPen);

		for (int track = 0; track <= tracks; heightSoFar += (trackHeights[track] + separatorHeight), track++)
		{
			//if (track > 0)
			{
				p.drawRect(0, heightSoFar, this->width(), separatorHeight);
			}
		}
	}

	//draw track items
	{
		qreal heightSoFar = headerHeight + separatorHeight;

		for (int track = 0; track < tracks; heightSoFar += (trackHeights[track] + separatorHeight), track++)
		{
			Timeline items = this->post->getTimeline(track);

			int line = 0;
			Timeline openItems;

			for (int i = 0; i < items.size(); i++)
			{
				TimelineItem *item = items[i];

				//remove all clips that has finished at this point
				QMutableListIterator<TimelineItem *> iter(openItems);
				while (iter.hasNext())
				{
					if (iter.next()->to <= item->from)
					{
						iter.remove();
					}
				}

				/*
				//add clips on top of each other. maximum overlays counting won't cut it
				int maxLine = 0;
				foreach (Clip *clip, openClips)
				{
					if (clip->line > maxLine)
						maxLine = clip->line;
				}
				line = openClips.empty() ? 0 : maxLine + 1;
				*/

				//find first empty line
				for (int j = 0; j < openItems.count() + 1; j++)
				{
					bool found = false;
					foreach (TimelineItem *item, openItems)
					{
						if (item->line == j)
						{
							found = true;
							break;
						}
					}

					if (!found)
					{
						line = j;
						break;
					}
				}

				item->line = line;
				//qDebug() << item->name << item->line;

				qreal x = msToPixels(item->from, duration);
				qreal y = heightSoFar + (qreal) line * lineHeights[track];
				qreal w = -1.0 + msToPixels(item->to - item->from, duration, false);
				qreal h = lineHeights[track] - 1.0;

				int alpha = 64;

				//for effects, modulate the opacity by on/off status
				PostEffect *effect = dynamic_cast<PostEffect *> (item);
				if (effect != 0)
				{
					if (effect->visible)
						alpha = 255;
				}


				p.setPen(QColor(0,0,20,255));
				QColor clipColor = colors[i % colors.size()];
				QColor transp = QColor(qRed(clipColor.rgb()), qGreen(clipColor.rgb()), qBlue(clipColor.rgb()), alpha);
				p.setBrush(transp);

				if (effect != 0)
				{
					p.setBrush(QColor(64, 64, 96, effect->visible ? 255 : 64));
				}

				if (item->adjustment)
				{
					p.setBrush(QColor(64, 64, 96, item->visible ? 64 : 64));
				}

				p.drawRect(x,
						   y,
						   w,
						   h);

				/*if (item->adjustment)
				{
					QBrush b;
					b.setStyle(Qt::BDiagPattern);
					b.setColor(QColor(255, 255, 255, item->visible ? 64 : 32));
					p.setBrush(b);

					p.drawRect(x,
							   y,
							   w,
							   h);
				}*/


				//for clips, draw clip opacity and speed
				Clip *clip = dynamic_cast<Clip *> (item);
				if (clip != 0)
				{
					int res = 512;

					//opacity
					QPolygonF opacity;
					for (int j = 0; j < res; j++)
					{
						qreal zo = (qreal) j / (qreal) (res - 1);
						qreal v = clip->getAlpha(pixelsToMs(x + zo * (w - 1), duration));

						//qDebug() << v << pixelsToMs(x + zo * (w - 1), duration) << x + zo * (w - 1);

						opacity << QPointF(x + zo * (w - 1), y + (h - 1.0) * (1.0 - v));
					}
					opacity << QPointF(x + w - 1.0, y + h - 1.0);
					opacity << QPointF(x, y + h - 1.0);

					p.setPen(Qt::NoPen);
					p.setBrush(item->adjustment ? QColor(64, 64, 96, item->visible ? 255 : 0) : clipColor);
					p.drawPolygon(opacity);

					if (item->adjustment)
					{
						QBrush b;
						b.setStyle(Qt::BDiagPattern);
						b.setColor(QColor(255, 255, 255, item->visible ? 64 : 32));
						p.setBrush(b);
						p.drawPolygon(opacity);
					}

					//...and speed
					QPolygonF speed;
					for (int j = 0; j < res; j++)
					{
						qreal zo = (qreal) j / (qreal) (res - 1);
						qreal v = clip->getSpeed(pixelsToMs(x + zo * (w - 1), duration));

						speed << QPointF(x + zo * (w - 1), y + (h - 1.0) * (1.0 - v));
					}

					p.setPen(QColor(255, 255, 255, 128));
					p.drawPolyline(speed);

				}

				//draw titles
				qreal realX0 = qMax(0.0, x);
				qreal realX1 = qMin((qreal) this->width(), x + w);
				qreal realW = realX1 - realX0;

				if (realW > 0 && drawText)
				{
					p.setPen(QColor(255,255,255,128));
					p.drawText(realX0,
							   y,
							   realW,
							   h,
							   Qt::AlignCenter | Qt::AlignVCenter,
							   item->name
							   );
				}


				openItems << item;
			}
		}
	}

	//draw position
	qreal cursor = msToPixels(position, duration) + 0.5;

	/*QPen whitePen(QColor(255, 255, 255, 255));
	whitePen.setWidth(3);
	p.setPen(whitePen);
	p.drawLine(cursor,
			   0.0,
			   cursor,
			   (qreal) this->height());*/

	//p.setPen(QColor(0, 0, 20, 255));
	p.setPen(QColor(255, 255, 255, 255));
	p.drawLine(cursor,
			   0.0,
			   cursor,
			   (qreal) this->height());
	QPolygonF triangle;
	triangle << QPointF(cursor, this->height() - 5)
			 << QPointF(cursor + 5, this->height())
			 << QPointF(cursor - 5, this->height());

	p.setPen(Qt::NoPen);
	//p.setBrush(QColor(0, 0, 20, 255));
	p.setBrush(QColor(255, 255, 255, 255));
	p.drawPolygon(triangle);

	//draw header
	msSoFar = 0;
	bool showHeader = true;
	foreach (SoundtrackSection *section, this->post->getSoundtrack()->getSections())
	{
		p.setPen(QColor(0,0,20,255));
		if (showHeader)
			p.setBrush(section->color);
		else
			p.setBrush(QColor(64,64,64,255));

		p.drawRoundedRect(msToPixels(msSoFar, duration),
						  1.0,
						  msToPixels(section->duration, duration, false) - 1,
						  headerHeight - 1.0,
						  headerHeight / 4.0,
						  headerHeight / 4.0);

		p.setPen(QColor(30,30,35,255));

		if (showHeader && drawText)
		{
			p.drawText(msToPixels(msSoFar, duration),
					   1.0,
					   msToPixels(section->duration, duration, false) - 1,
					   headerHeight - 1.0,
					   Qt::AlignCenter | Qt::AlignVCenter,
					   section->description
					   );
		}

		msSoFar += section->duration;
	}
}
