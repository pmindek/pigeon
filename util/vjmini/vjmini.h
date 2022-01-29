/* =======================================
 *                    Vj
 *                 v2018.01
 *
 *  Copyright (C) 2018  Nanographics GmbH
 *           All Rights Reserved
 * =======================================
 *
 * This library provides functionality for
 * compositing different visual resources,
 * such as visualizations or visual
 * effects, and providing real-time and
 * offline output.
 *
 */

#ifndef VJ_H
#define VJ_H

#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHash>
#include <QEasingCurve>
#include <QStringList>
#include <QFileSystemWatcher>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QSurface>
#include <QVector2D>

#include "nn.h"

#include "sources/parameter.h"

//TODO: remove this dependancy. VJ should be just injected all the renderers/providers. think of what to do with
//special cases - shaderprovider and procedurerenderer
#include "sources/mappingmanager.h"
#include "sources/shadermanager.h"
#include "sources/textureprovider.h"

//TODO: separate file
template <class T> class Keyframe
{
public:
	inline Keyframe(qreal ms, QEasingCurve::Type et, T data, qreal lc)
	{
		this->ms = ms;
		this->et = et;
		this->data = data;
		this->lc = lc;
	}
	QEasingCurve::Type et;
	qreal ms;
	qreal lc; //linear-cubic interpolation interpolation
	T data;
};

class VJMini : public QObject
{
	Q_OBJECT
public:
	NN *nn;

	explicit VJMini(QObject *parent = 0);
	~VJMini();

	inline GLenum getBlendingFactorFromString(QString factorString)
	{
		GLenum factor = 0;
		QString factorStringTrimmed = factorString.toUpper().trimmed();

		if (!factorStringTrimmed.startsWith("GL_"))
		{
			factorStringTrimmed = "GL_" + factorStringTrimmed;
		}

		if (factorStringTrimmed == "GL_ZERO")
			factor = GL_ZERO;
		if (factorStringTrimmed == "GL_ONE")
			factor = GL_ONE;
		if (factorStringTrimmed == "GL_SRC_COLOR")
			factor = GL_SRC_COLOR;
		if (factorStringTrimmed == "GL_ONE_MINUS_SRC_COLOR")
			factor = GL_ONE_MINUS_SRC_COLOR;
		if (factorStringTrimmed == "GL_DST_COLOR")
			factor = GL_DST_COLOR;
		if (factorStringTrimmed == "GL_ONE_MINUS_DST_COLOR")
			factor = GL_ONE_MINUS_DST_COLOR;
		if (factorStringTrimmed == "GL_SRC_ALPHA")
			factor = GL_SRC_ALPHA;
		if (factorStringTrimmed == "GL_ONE_MINUS_SRC_ALPHA")
			factor = GL_ONE_MINUS_SRC_ALPHA;
		if (factorStringTrimmed == "GL_DST_ALPHA")
			factor = GL_DST_ALPHA;
		if (factorStringTrimmed == "GL_ONE_MINUS_DST_ALPHA")
			factor = GL_ONE_MINUS_DST_ALPHA;
		if (factorStringTrimmed == "GL_CONSTANT_COLOR")
			factor = GL_CONSTANT_COLOR;
		if (factorStringTrimmed == "GL_ONE_MINUS_CONSTANT_COLOR")
			factor = GL_ONE_MINUS_CONSTANT_COLOR;
		if (factorStringTrimmed == "GL_CONSTANT_ALPHA")
			factor = GL_CONSTANT_ALPHA;
		if (factorStringTrimmed == "GL_ONE_MINUS_CONSTANT_ALPHA")
			factor = GL_ONE_MINUS_CONSTANT_ALPHA;
		if (factorStringTrimmed == "GL_SRC_ALPHA_SATURATE")
			factor = GL_SRC_ALPHA_SATURATE;
		if (factorStringTrimmed == "GL_SRC1_COLOR")
			factor = GL_SRC1_COLOR;
		if (factorStringTrimmed == "GL_ONE_MINUS_SRC1_COLOR")
			factor = GL_ONE_MINUS_SRC1_COLOR;
		if (factorStringTrimmed == "GL_SRC1_ALPHA")
			factor = GL_SRC1_ALPHA;
		if (factorStringTrimmed == "GL_ONE_MINUS_SRC1_ALPHA")
			factor = GL_ONE_MINUS_SRC1_ALPHA;

		return factor;
	}

	template<typename T> inline T getAnimatedParameter(qreal ms, qreal stopAt, qreal linearCubic, QList<Keyframe<T> *> animation)
	{
		//qDebug() << ms << stopAt << linearCubic;
		bool endPad = true;
		int currentKeyframeIndex = animation.count() - 1;

		for (int i = 0; i < animation.count(); i++)
		{
			if (animation[i]->ms > ms)
			{
				currentKeyframeIndex = i - 1;
				endPad = false;
				break;
			}
		}

		bool frontPad = false;
		if (currentKeyframeIndex < 0)
		{
			currentKeyframeIndex = 0;
			frontPad = true;
		}

		int i0 = qMax(0, currentKeyframeIndex-1);
		int i1 = currentKeyframeIndex;
		int i2 = qMin(currentKeyframeIndex+1, animation.count() - 1);
		int i3 = qMin(currentKeyframeIndex+2, animation.count() - 1);

		QEasingCurve e(animation[currentKeyframeIndex]->et);
		qreal x = stopAt * e.valueForProgress( (ms - animation[i1]->ms) / (animation[i2]->ms - animation[i1]->ms) );

		if (frontPad)
		{
			x = 0.0;
		}

		if (endPad)
		{
			x = 1.0;
		}

		T m0 = NNU::mix(animation[i0]->data,
							 animation[i1]->data,
							 animation[i2]->data,
							 animation[i3]->data,
							 x);

		//qDebug() << "m0=" << m0;

		T m1 = NNU::mix(
							 animation[i1]->data,
							 animation[i2]->data,
							 x);
		//qDebug() << "m1=" << m1;


		qreal interpolatedLinearCubic = NNU::mix(
												 animation[i1]->lc,
												 animation[i2]->lc,
												 x);

		//qDebug() << "x=" << x << "result = " <<  NNU::mix(m1, m0, linearCubic * interpolatedLinearCubic);

		return NNU::mix(m1, m0, linearCubic * interpolatedLinearCubic);
	}

	bool loadScript(QString fileName);

	//TODO: move to nn? same as with nn->fbos - have one set of string shaders, one set of int shaders
	inline NNShader *shader(QString key)
	{
		return VPointer<NNShader>::toPointer(this->shaderManager->getResources()->get(key));
	}

	inline GLuint texture(QString key)
	{
		if (!this->textureProvider->contains(key))
		{
			return 0;
		}

		return VPointer<QOpenGLTexture>::toPointer(this->textureProvider->get(key))->textureId();
	}

	inline QOpenGLTexture *textureObject(QString key)
	{
		return VPointer<QOpenGLTexture>::toPointer(this->textureProvider->get(key));
	}

	inline QHash<QString, QVariant> getTextures()
	{
		return this->textureProvider->getResources()->getResources();
	}

	inline QHash<QString, QVariant> getShaders()
	{
		return this->shaderManager->getResources()->getResources();
	}

	inline QHash<QString, QVariant> getMappings()
	{
		return this->mappingManager->getResources()->getResources();
	}


	inline void setContext(QOpenGLContext *context)
	{
		this->context = context;

		if (this->offScreenSurface != 0)
		{
			delete this->offScreenSurface;
			this->offScreenSurface = 0;
		}

		this->offScreenSurface = new QOffscreenSurface();
		this->offScreenSurface->setFormat(this->context->format());
		this->offScreenSurface->create();
	}

	inline QOpenGLContext *getContext()
	{
		return this->context;
	}

	inline void setSurface(QSurface *onScreenSurface)
	{
		this->onScreenSurface = onScreenSurface;
	}

	TextureProvider *textureProvider;
	MappingManager *mappingManager;
	ShaderManager *shaderManager;

	inline bool cExists(QString key)
	{
		return this->config.contains(key);
	}

	inline QHash<QString, QVariant> & getConfig()
	{
		return this->config;
	}

	inline QVariant c(QString key)
	{
		return this->config[key];
	}

	inline qreal ci(QString key)
	{
		return this->config[key].toInt();
	}

	inline qreal cr(QString key)
	{
		return this->config[key].toDouble();
	}

	inline QString cs(QString key)
	{
		return this->config[key].toString();
	}

	inline QStringList csl(QString key)
	{
		return this->config[key].toStringList();
	}

	inline QVector2D cv2(QString key)
	{
		return qvariant_cast<QVector2D> (this->config[key]);
	}

	inline QVector3D cv3(QString key)
	{
		return qvariant_cast<QVector3D> (this->config[key]);
	}

	inline QVector4D cv4(QString key)
	{
		return qvariant_cast<QVector4D> (this->config[key]);
	}

	inline QMatrix4x4 cm4x4(QString key)
	{
		return qvariant_cast<QMatrix4x4> (this->config[key]);
	}

private:
	QFileSystemWatcher *scriptWatcher;

	QJsonDocument loadJson(QString fileName);
	void finalizeScriptFile(QString fileName);

	QString scriptFileName;
	QString resourcesFileName;
	QString configFileName;

	QList<ResourceManager *> resourceManagers;
	QList<ResourceRenderer *> resourceRenderers;

	QString tempFbo;
	QString outputFbo;
	QString pingFbo;
	QString pongFbo;

	QSurface *onScreenSurface;
	QOffscreenSurface *offScreenSurface;
	QOpenGLContext *context;

	QHash<QString, QVariant> config;

signals:
	void updated();

public slots:
	void reloadScriptFile(QString);

	bool loadScriptFile(QString fileName);
	bool loadResources(QString fileName);
	bool loadConfig(QString fileName);
};

#endif // VJ_H
