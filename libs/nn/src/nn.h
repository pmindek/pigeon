/* =======================================
 *                    NN
 *                 v2018.02
 *
 *  Copyright (C) 2018  Nanographics GmbH
 *           All Rights Reserved
 * =======================================
 *
 * This library provides functionality for
 * graphics programming. It contains code
 * for managing resources, such as shaders
 * and buffers, as well as basic rendering
 * functionality.
 *
 */

#ifndef NN_H
#define NN_H

#include <QObject>

#include "nn_global.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLFramebufferObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#include "resources.nn/nnpagedarray.h"
#include "resources.nn/nnshader.h"
#include "resources.nn/nnresource.h"

#include "util.nn/Perlin.h"
#include "util.nn/SimplexNoise.h"

#include "nnu.h"

//#define OPENGL_FUNCTIONS QOpenGLFunctions_4_5_Core
#define OPENGL_FUNCTIONS QOpenGLFunctions_2_0

typedef QSharedPointer<QOpenGLFramebufferObject> PFBO;
typedef QSharedPointer<QOpenGLTexture> PTexture;
typedef QSharedPointer<QList<GLenum> > PLENUM;
#define PNULL QSharedPointer<QOpenGLFramebufferObject>()

class NN_API NNFboSettings
{
public:
	bool fixedSize;

};

class NN_API NN : public QObject, public OPENGL_FUNCTIONS
{
	Q_OBJECT
public:
	explicit NN(QObject *parent = nullptr);

	inline int year()
	{
		return versionYear;
	}
	inline int build()
	{
		return versionBuild;
	}
	inline QString version()
	{
		return QString::number(versionYear) + "." + QString::number(versionBuild);
	}

	void init();
	void resize(int w, int h);

	//rendering
	void drawFullScreenQuad(GLuint texture, qreal alpha, qreal zoom, bool flipHorizontal, bool flipVertical, QVector2D pan = QVector2D());
	void drawFullScreenQuad(GLuint texture, bool flipHorizontal, bool flipVertical, QVector2D pan = QVector2D());
	void drawFullScreenQuad(qreal alpha, qreal zoom, bool flipHorizontal, bool flipVertical, QVector2D pan = QVector2D());
	void drawFullScreenQuad(qreal zoom, bool flipHorizontal, bool flipVertical, QVector2D pan = QVector2D());

	//FBOs
	PFBO createFbo(int width, int height, GLenum internalFormat);

	bool addFbo(QString key);
	bool addFbo(QString key, GLenum internalFormat);
	bool addFbo(int key);
	bool addFbo(int key, GLenum internalFormat);
	bool addColorAttachment(QString key, GLenum internalFormat);
	bool addColorAttachment(int key, GLenum internalFormat);
	PFBO fbo(QString key);
	PFBO fbo(int key);
	void clearFbos();
	void dumpFbos(QString path);
	void listFbos();

	NNResource<QString, PFBO> fbos;
	NNResource<int, PFBO> iFbos;
	NNResource<QString, QSharedPointer<NNShader> > shaders;
	NNResource<int, QSharedPointer<NNShader> > iShaders;
	NNResource<QString, QSharedPointer<QOpenGLBuffer> > buffers;
	NNResource<int, QSharedPointer<QOpenGLBuffer> > iBuffers;
	NNResource<QString, QSharedPointer<QOpenGLTexture> > textures;
	NNResource<int, QSharedPointer<QOpenGLTexture> > iTextures;

private:
	int dumpFbosCounter;

	int versionYear;
	int versionBuild;

	QSize outputSize;

	QHash<QString, GLenum> fboInternalFormats;
	QHash<int, GLenum> iFboInternalFormats;
	QHash<QString, PLENUM> fboColorAttachments;
	QHash<int, PLENUM> iFboColorAttachments;

signals:

public slots:
};

#endif // NN_H
