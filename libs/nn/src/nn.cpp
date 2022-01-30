#include "nn.h"
#include "util.nn/gizmos.h"


NN::NN(QObject *parent) : QObject(parent)
{
	versionYear = 2022;
	versionBuild = 1;

	dumpFbosCounter = 0;
}

//initializes NN
void NN::init()
{
    bool success = this->initializeOpenGLFunctions();
	/*QOpenGLFunctions_4_3_Core *gl43 = new QOpenGLFunctions_4_3_Core();
    gl43->initializeOpenGLFunctions();
    Gizmos::setGL(gl43);
    if(!success)
    {
        qDebug() << "not able to initialize OpenGL Functions";
	}*/

	//this->outputSize = QSize(w, h);
}

void NN::resize(int w, int h)
{
	qDebug() << "NN: resize is called:" << w << h;

	qDebug() << this->fbos.count() << this->iFbos.count();
	this->outputSize = QSize(w, h);

	NNResource<QString, PFBO> temporaryFbos;
	QHashIterator<QString, PFBO> iter(this->fbos.getResources());
	while (iter.hasNext())
	{
		iter.next();

		/*QOpenGLFramebufferObjectFormat format;
		format.setAttachment(i.value()->attachment());
		format.setMipmap(true);
		format.setSamples(32);
		format.setTextureTarget(GL_TEXTURE_2D);
		format.setInternalTextureFormat(GL_RGBA32F_ARB);
		QOpenGLFramebufferObject *fbo = new QOpenGLFramebufferObject(w, h, format);*/

		PFBO fbo = PFBO(new QOpenGLFramebufferObject(w, h, iter.value()->attachment(), GL_TEXTURE_2D, this->fboInternalFormats[iter.key()]));

		glBindTexture(GL_TEXTURE_2D, fbo->texture());
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		foreach (GLenum internalFormat, *this->fboColorAttachments.value(iter.key()))
		{
			fbo->addColorAttachment(w, h, internalFormat);
		}

		temporaryFbos.add(iter.key(), fbo);
	}

	this->fbos.clear();
	//TODO: copy contructor
	this->fbos.set(temporaryFbos.getResources());



	NNResource<int, PFBO > iTemporaryFbos;
	QHashIterator<int, PFBO > iIter(this->iFbos.getResources());
	while (iIter.hasNext())
	{
		iIter.next();

		PFBO fbo = PFBO(new QOpenGLFramebufferObject(w, h, iIter.value()->attachment(), GL_TEXTURE_2D, this->iFboInternalFormats[iIter.key()]));

		glBindTexture(GL_TEXTURE_2D, fbo->texture());
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		fbo->bind();
		foreach (GLenum internalFormat, *this->iFboColorAttachments.value(iIter.key()))
		{
			fbo->addColorAttachment(w, h, internalFormat);
		}
		fbo->release();

		iTemporaryFbos.add(iIter.key(), fbo);
	}

	this->iFbos.clear();
	//TODO: copy contructor
	this->iFbos.set(iTemporaryFbos.getResources());
	this->fbos.set(temporaryFbos.getResources());

	qDebug() << this->fbos.count() << this->iFbos.count();
}



//TODO: get rid of immediate mode
void NN::drawFullScreenQuad(GLuint texture, qreal alpha, qreal zoom, bool flipHorizontal, bool flipVertical, QVector2D pan)
{
	this->glBindTexture(GL_TEXTURE_2D, texture);
	this->glColor4f(1.0, 1.0, 1.0, alpha);
	this->drawFullScreenQuad(zoom, flipHorizontal, flipVertical, pan);
}
void NN::drawFullScreenQuad(GLuint texture, bool flipHorizontal, bool flipVertical, QVector2D pan)
{
	this->glBindTexture(GL_TEXTURE_2D, texture);
	this->drawFullScreenQuad(1.0, flipHorizontal, flipVertical, pan);
}
void NN::drawFullScreenQuad(qreal alpha, qreal zoom, bool flipHorizontal, bool flipVertical, QVector2D pan)
{
	this->glColor4f(1.0, 1.0, 1.0, alpha);
	this->drawFullScreenQuad(zoom, flipHorizontal, flipVertical, pan);
}
void NN::drawFullScreenQuad(qreal zoom, bool flipHorizontal, bool flipVertical, QVector2D pan)
{
	zoom = 1.0 / zoom;
	qreal _pan1D = (zoom - 1.0) * 0.5;

	QVector2D pan2D = QVector2D(_pan1D, _pan1D);

	pan2D += pan;

	this->glBegin(GL_QUADS);
		this->glTexCoord2f(flipHorizontal ? (1 - (-pan2D.x() + 0.0))        : (-pan2D.x() + 0.0),               flipVertical ? (1 - (-pan2D.y() + 0.0))        : (-pan2D.y() + 0.0)              ); this->glVertex3f(-1.0, -1.0, 0.0);
		this->glTexCoord2f(flipHorizontal ? (1 - (-pan2D.x() + 1.0 * zoom)) : (-pan2D.x() + 1.0 * zoom),        flipVertical ? (1 - (-pan2D.y() + 0.0))        : (-pan2D.y() + 0.0)              ); this->glVertex3f( 1.0, -1.0, 0.0);
		this->glTexCoord2f(flipHorizontal ? (1 - (-pan2D.x() + 1.0 * zoom)) : (-pan2D.x() + 1.0 * zoom),        flipVertical ? (1 - (-pan2D.y() + 1.0 * zoom)) : (-pan2D.y() + 1.0 * zoom)       ); this->glVertex3f( 1.0,  1.0, 0.0);
		this->glTexCoord2f(flipHorizontal ? (1 - (-pan2D.x() + 0.0))        : (-pan2D.x() + 0.0),               flipVertical ? (1 - (-pan2D.y() + 1.0 * zoom)) : (-pan2D.y() + 1.0 * zoom)       ); this->glVertex3f(-1.0,  1.0, 0.0);
	this->glEnd();
}




//creates and returns a fbo of the given size
PFBO NN::createFbo(int width, int height, GLenum internalFormat)
{
	PFBO fbo = PFBO(new QOpenGLFramebufferObject(width, height, QOpenGLFramebufferObject::CombinedDepthStencil, GL_TEXTURE_2D, internalFormat));
    glGetError();
    qDebug() << "OpenGL error:" << glGetError();
    GLuint texID = fbo->texture();
	this->glBindTexture(GL_TEXTURE_2D, fbo->texture());
	this->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	this->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	this->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	fbo->release();

	return fbo;
}

//FBOs

//adds a fbo to the string-based directory
bool NN::addFbo(QString key)
{
	return this->addFbo(key, GL_RGBA/*32F*/);
}

bool NN::addFbo(QString key, GLenum internalFormat)
{
	if (fbos.contains(key))
	{
		return false;
	}

	qDebug() << "NN: creating string fbo" << this->outputSize;
	PFBO fbo = createFbo(this->outputSize.width(), this->outputSize.height(), internalFormat);

	if (!this->fbos.add(key, fbo))
	{
		//no need to delete, as fbo is a shared pointer
		return false;
	}
	else
	{
		this->fboInternalFormats.insert(key, internalFormat);
		this->fboColorAttachments.insert(key, PLENUM(new QList<GLenum>));

		return true;
	}
}

//adds a fbo to the int-based directory
bool NN::addFbo(int key)
{
	return this->addFbo(key, GL_RGBA/*32F*/);
}

bool NN::addFbo(int key, GLenum internalFormat)
{
	if (iFbos.contains(key))
	{
		return false;
	}

	qDebug() << "NN: creating int fbo" << this->outputSize;
	PFBO fbo = createFbo(this->outputSize.width(), this->outputSize.height(), internalFormat);

	if (!this->iFbos.add(key, fbo))
	{
		//no need to delete, as fbo is a shared pointer
		return false;
	}
	else
	{
		this->iFboInternalFormats.insert(key, internalFormat);
		this->iFboColorAttachments.insert(key, PLENUM(new QList<GLenum>));

		return true;
	}
}

//adds an extra color attachment (in addition to a default color attachment which every fbo has) to a fbo identified by the key
bool NN::addColorAttachment(QString key, GLenum internalFormat)
{
	if (fbos.contains(key))
	{
		this->fbos[key]->bind();
		this->fbos[key]->addColorAttachment(this->outputSize.width(), this->outputSize.height(), internalFormat);
		this->fbos[key]->release();
		this->fboColorAttachments.value(key)->append(internalFormat);
		return true;
	}
	else
	{
		return false;
	}
}

//overloaded function for the int-based directory of fbos
bool NN::addColorAttachment(int key, GLenum internalFormat)
{
	if (iFbos.contains(key))
	{
		this->iFbos[key]->bind();
		this->iFbos[key]->addColorAttachment(this->outputSize.width(), this->outputSize.height(), internalFormat);
		this->iFboColorAttachments.value(key)->append(internalFormat);
		this->iFbos[key]->release();
		return true;
	}
	else
	{
		return false;
	}
}

PFBO NN::fbo(QString key)
{
	return this->fbos[key];
}

PFBO NN::fbo(int key)
{
	return this->iFbos[key];
}

//deletes all fbos from the string-based dictionary (since they are stored as smart pointers, they also get deleted)
void NN::clearFbos()
{
	this->fbos.clear();
	this->iFbos.clear();
	this->fboColorAttachments.clear();
	this->iFboColorAttachments.clear();
	this->fboInternalFormats.clear();
	this->iFboInternalFormats.clear();
}

//dumps all fbos into png files
void NN::dumpFbos(QString path)
{
	if (!path.endsWith("/"))
		path = path + "/";

	QHashIterator<QString, PFBO > is(this->fbos.getResources());
	while (is.hasNext())
	{
		is.next();

		is.value()->toImage().save(path + QString::number(dumpFbosCounter) + " nn-" + "-" + is.key() + ".png");
	}

	QHashIterator<int, PFBO > ii(this->iFbos.getResources());
	while (ii.hasNext())
	{
		ii.next();

		ii.value()->toImage().save(path + QString::number(dumpFbosCounter) + " nn-i-" + "-" + QString::number(ii.key()) + ".png");
	}

	dumpFbosCounter++;
}

//print all available fbos
void NN::listFbos()
{
	qDebug() << "NN: string FBOs:";
	QHashIterator<QString, PFBO > is(this->fbos.getResources());
	while (is.hasNext())
	{
		is.next();

		qDebug() << is.key() << is.value();
	}

	qDebug() << "NN: int FBOs:";
	QHashIterator<int, PFBO > ii(this->iFbos.getResources());
	while (ii.hasNext())
	{
		ii.next();

		qDebug() << ii.key() << ii.value();
	}
}
//SHADERS



//PRIVATE

