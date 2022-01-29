#ifndef RESOURCERENDERER_H
#define RESOURCERENDERER_H

#include <QtGlobal>
#include <QOpenGLTexture>
#include <QOpenGLFunctions_2_0>

#include "nn.h"

#include "parameter.h"

class ResourceRenderer
{
public:
	ResourceRenderer(QString name);

	inline virtual void initFrame(qreal absoluteMs)
	{

	}

	inline virtual void finalizeFrame()
	{

	}

	inline virtual void resize(int width, int height)
	{
		this->size = QSize(width, height);
	}

	virtual PFBO getFrame(QString resource, quint64 absoluteMs, quint64 ms, quint64 previousMs, qreal relativePosition, qreal previousRelativePosition) = 0;
	virtual bool init(NN *nn, QHash<QString, QVariant> settings);
	virtual void setFrameOptions(QHash<QString, QVariant> options);
	virtual void setFrameParameters(NNResource<QString, QSharedPointer<Parameter> > parameters);
	virtual void setFrameTextures(QHash<QString, GLuint> textures);
	QList<PFBO> getLayers();
	PFBO getLayer(int index);
	int getLayerCount();
	PFBO getCurrentLayer();
	int getCurrentLayerIndex();
	void newFrame();
	PFBO nextLayer();
	QString getName();

protected:
	QString name;

	int layer;

	NN *nn;

	QList<PFBO> layers;
	//QList<PFBO> panels;
	PFBO panels;

	QHash<QString, QVariant> frameOptions;
	NNResource<QString, QSharedPointer<Parameter> > frameParameters;
	QHash<QString, GLuint> frameTextures;

	int maxOverlays;
	QSize size;
};

#endif // RESOURCERENDERER_H
