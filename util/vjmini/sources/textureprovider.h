#ifndef TEXTUREPROVIDER_H
#define TEXTUREPROVIDER_H


#include <QObject>
#include <QImage>
#include <QOpenGLTexture>
#include "resourceprovider.h"

class TextureProvider : public ResourceProvider
{
	Q_OBJECT
public:
	inline TextureProvider(QString name, QObject *parent = 0) : ResourceProvider(name, parent)
	{
		//textureFormat = QOpenGLTexture::RGBA8_UNorm;
		// TODO: check if compressed texture format works properly and if it is passed to OpenGL
		textureFormat = QOpenGLTexture::RGBA_ASTC_4x4;

	}

	inline void setTextureFormat(QOpenGLTexture::TextureFormat format)
	{
		textureFormat = format;
	}

	bool init(NN *nn, QHash<QString, QVariant> settings);

	inline virtual PFBO getFrame(QString resource, quint64 absoluteMs, quint64 ms, quint64 previousMs, qreal relativePosition, qreal previousRelativePosition)
	{
		int currentLayer = this->getCurrentLayerIndex();
		this->layers[currentLayer]->bind();

		this->nn->glClearColor(0,0,0,0);
		this->nn->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->nn->drawFullScreenQuad(VPointer<QOpenGLTexture>::toPointer(this->resources[resource])->textureId(), 1.0, 1.0, false, true);

		this->layers[currentLayer]->release();

		return this->nextLayer();
	}

	inline virtual void clear()
	{
		QHashIterator<QString, QVariant> i(this->resources.getResources());
		while (i.hasNext())
		{
			i.next();
			qDebug() << "deleting texture" << i.key();
			delete VPointer<QOpenGLTexture>::toPointer(i.value());
		}

		ResourceManager::clear();
	}

	inline virtual bool add(QString key, QStringList fileNames)
	{
		if (fileNames.empty())
		{
			return false;
		}

		QString textureFileName = filePath + fileNames[0];

		QImage image;
		image.load(textureFileName);

		qDebug() << textureFileName << "has dimensions of" << image.size();

		/*QOpenGLTexture *texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
		texture->create();
		texture->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::LinearMipMapLinear);
		texture->setAutoMipMapGenerationEnabled(true);
		texture->setSize(image.width(), image.height());
		texture->setFormat(textureFormat);
		texture->setWrapMode(QOpenGLTexture::ClampToEdge);
		texture->setMaximumAnisotropy(texture->maximumAnisotropy());

		//texture->allocateStorage(QOpenGLTexture::BGRA, QOpenGLTexture::UInt8);
		texture->setData(image);*/
		QOpenGLTexture *texture = new QOpenGLTexture(image.mirrored());
		texture->setMagnificationFilter(QOpenGLTexture::Linear);
		texture->setMinificationFilter(QOpenGLTexture::Linear);


		if (!this->resources.add(key, VPointer<QOpenGLTexture>::toVariant(texture), textureFileName))
		{
			delete texture;
			return false;
		}
		else
		{
			this->watchFiles(textureFileName);

			return true;
		}
	}

public slots:
	inline void resourceReloaded(QString fileName)
	{
		QFile file(fileName);

		int counter = 0;
		while (!file.open(QIODevice::ReadOnly))
		{
			qDebug() << "Trying to open" << fileName << "...";
			counter++;

			QThread::msleep(100);

			if (counter > 50)
			{
				break;
			}
		}

		QImage image(fileName);

		if (!image.isNull())
		{
			QString key = this->resources.getKeyFromFileName(fileName);
			QOpenGLTexture *texture = VPointer<QOpenGLTexture>::toPointer(this->resources[key]);

			texture->destroy();
			texture->setData(image.mirrored());

			emit resourceUpdated();
		}
	}

private:
	QOpenGLTexture::TextureFormat textureFormat;

};
#endif // TEXTUREPROVIDER_H
