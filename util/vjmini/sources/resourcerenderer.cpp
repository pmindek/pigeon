#include "resourcerenderer.h"

ResourceRenderer::ResourceRenderer(QString name) : name(name)
{

}

bool ResourceRenderer::init(NN *nn, QHash<QString, QVariant> settings)
{
	this->maxOverlays = settings["maxOverlays"].toInt();
	this->size = settings["size"].toSize();
	this->nn = nn;

	this->layer = 0;

	return true;
}

//TODO: remove; use frameParameters instead
void ResourceRenderer::setFrameOptions(QHash<QString, QVariant> options)
{
	this->frameOptions = options;
}

void ResourceRenderer::setFrameParameters(NNResource<QString, QSharedPointer<Parameter> > parameters)
{
	this->frameParameters = parameters;
}

void ResourceRenderer::setFrameTextures(QHash<QString, GLuint> textures)
{
	this->frameTextures = textures;
}

QList<PFBO> ResourceRenderer::getLayers()
{
	return layers;
}

PFBO ResourceRenderer::getLayer(int index)
{
	if (index >= this->layers.count())
	{
		return 0;
	}
	else
	{
		return layers[index];
	}
}

int ResourceRenderer::getLayerCount()
{
	return layers.count();
}

PFBO ResourceRenderer::getCurrentLayer()
{
	return this->getLayer(layer);
}

int ResourceRenderer::getCurrentLayerIndex()
{
	return layer;
}

void ResourceRenderer::newFrame()
{
	this->layer = 0;
}

//increments the layer index and returns the shared pointer to an FBO of the layer which was current when the function has been called
PFBO ResourceRenderer::nextLayer()
{
	int currentLayer = this->layer;
	this->layer++;
	return this->getLayer(currentLayer);
}

QString ResourceRenderer::getName()
{
	return this->name;
}
