#include "textureprovider.h"

bool TextureProvider::init(NN *nn, QHash<QString, QVariant> settings)
{
	ResourceRenderer::init(nn, settings);

	layers.clear();

	this->layer = 0;

	for (int i = 0; i < maxOverlays; i++)
	{
		PFBO layer = PFBO(new QOpenGLFramebufferObject(this->size.width(), this->size.height(), QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D));

		nn->glBindTexture(GL_TEXTURE_2D, layer->texture());
		nn->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		nn->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		nn->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		nn->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		layer->release();

		this->layers << layer;
	}

	return nn->glGetError() == 0;
}
