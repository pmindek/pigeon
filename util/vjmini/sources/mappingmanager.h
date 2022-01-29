#ifndef MAPPINGMANAGER_H
#define MAPPINGMANAGER_H

#include <QObject>
#include <QFile>
#include <QOpenGLTexture>
#include <QEasingCurve>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "resourcemanager.h"


class MappingManager : public ResourceManager
{
	Q_OBJECT
public:
	inline MappingManager(QString name, QObject *parent = 0) : ResourceManager(name, parent)
	{

	}

	inline virtual void clear()
	{
		QHashIterator<QString, QVariant> i(this->resources.getResources());
		while (i.hasNext())
		{
			i.next();
			qDebug() << "deleting mapping" << i.key();
			delete VPointer<QList<qreal> >::toPointer(i.value());
		}

		ResourceManager::clear();
	}


	inline virtual bool add(QString key, QStringList fileNames)
	{
		if (fileNames.empty())
		{
			return false;
		}

		QString fileName = filePath + fileNames[0];

		QList<qreal> *mapping = new QList<qreal>();

		if (!this->loadMapping(mapping, fileName))
		{
			delete mapping;
			return false;
		}

		if (!this->resources.add(key, VPointer<QList<qreal> >::toVariant(mapping), fileName))
		{
			mapping->clear();
			return false;
		}
		else
		{
			this->watchFiles(fileName);

			return true;
		}
	}

	inline qreal getMappingValue(QString key, qreal t)
	{
		//if the mapping with the given key exists, remap it
		if (resources.contains(key))
		{
			t = qBound(0.0, t, 1.0);
			QList<qreal> *mapping = VPointer<QList<qreal> >::toPointer(this->resources[key]);
			return mapping->at((int) ((qreal) (mapping->size() - 1) * t));

			return t;//this->resources.get(key).toList()[(int) ((qreal) (this->resources.get(key).toList().size() - 1) * t)].toDouble();
		}
		//if the mapping with the given key does not exist, fall back to linear mapping (x=y)
		else
		{
			return t;
		}
	}

private:
	bool loadMapping(QList<qreal> *mapping, QString fileName)
	{
		mapping->clear();

		QFile file(fileName);
		file.open(QIODevice::ReadOnly);

		if (file.isOpen())
		{
			QJsonDocument doc = QJsonDocument::fromJson(file.readAll());

			QJsonObject mappingObject = doc.object();

			QJsonArray curve = mappingObject["curve"].toArray();

			foreach (QJsonValue curveValue, curve)
			{
				mapping->append(curveValue.toDouble());
			}

			qDebug() << "loaded" << mapping->size() << "values";

			file.close();

			return true;
		}
		else
		{
			return false;
		}
	}

public slots:
	inline void resourceReloaded(QString fileName)
	{
		QString key = this->resources.getKeyFromFileName(fileName);
		QList<qreal> *mapping = VPointer<QList<qreal> >::toPointer(this->resources[key]);

		if (this->loadMapping(mapping, fileName))
		{
			this->resources.replace(key, VPointer<QList<qreal> >::toVariant(mapping));

			qDebug() << fileName << "reloaded" << mapping->count() << "values in" << key;

			if (mapping->count() > 0)
			{
				emit resourceUpdated();
			}
		}
	}

};

class EasingCurveDictionary
{
public:
	inline EasingCurveDictionary()
	{
		easingCurveTypes.insert("linear", QEasingCurve::Linear);
		easingCurveTypes.insert("inquad", QEasingCurve::InQuad);
		easingCurveTypes.insert("outquad", QEasingCurve::OutQuad);
		easingCurveTypes.insert("inoutquad", QEasingCurve::InOutQuad);
		easingCurveTypes.insert("outinquad", QEasingCurve::OutInQuad);
		easingCurveTypes.insert("incubic", QEasingCurve::InCubic);
		easingCurveTypes.insert("outcubic", QEasingCurve::OutCubic);
		easingCurveTypes.insert("inoutcubic", QEasingCurve::InOutCubic);
		easingCurveTypes.insert("outincubic", QEasingCurve::OutInCubic);
		easingCurveTypes.insert("inquart", QEasingCurve::InQuart);
		easingCurveTypes.insert("outquart", QEasingCurve::OutQuart);
		easingCurveTypes.insert("inoutquart", QEasingCurve::InOutQuart);
		easingCurveTypes.insert("outinquart", QEasingCurve::OutInQuart);
		easingCurveTypes.insert("inquint", QEasingCurve::InQuint);
		easingCurveTypes.insert("outquint", QEasingCurve::OutQuint);
		easingCurveTypes.insert("inoutquint", QEasingCurve::InOutQuint);
		easingCurveTypes.insert("outinquint", QEasingCurve::OutInQuint);
		easingCurveTypes.insert("insine", QEasingCurve::InSine);
		easingCurveTypes.insert("outsine", QEasingCurve::OutSine);
		easingCurveTypes.insert("inoutsine", QEasingCurve::InOutSine);
		easingCurveTypes.insert("outinsine", QEasingCurve::OutInSine);
		easingCurveTypes.insert("inexpo", QEasingCurve::InExpo);
		easingCurveTypes.insert("outexpo", QEasingCurve::OutExpo);
		easingCurveTypes.insert("inoutexpo", QEasingCurve::InOutExpo);
		easingCurveTypes.insert("outinexpo", QEasingCurve::OutInExpo);
		easingCurveTypes.insert("incirc", QEasingCurve::InCirc);
		easingCurveTypes.insert("outcirc", QEasingCurve::OutCirc);
		easingCurveTypes.insert("inoutcirc", QEasingCurve::InOutCirc);
		easingCurveTypes.insert("outincirc", QEasingCurve::OutInCirc);
		easingCurveTypes.insert("inelastic", QEasingCurve::InElastic);
		easingCurveTypes.insert("outelastic", QEasingCurve::OutElastic);
		easingCurveTypes.insert("inoutelastic", QEasingCurve::InOutElastic);
		easingCurveTypes.insert("outinelastic", QEasingCurve::OutInElastic);
		easingCurveTypes.insert("inback", QEasingCurve::InBack);
		easingCurveTypes.insert("outback", QEasingCurve::OutBack);
		easingCurveTypes.insert("inoutback", QEasingCurve::InOutBack);
		easingCurveTypes.insert("outinback", QEasingCurve::OutInBack);
		easingCurveTypes.insert("inbounce", QEasingCurve::InBounce);
		easingCurveTypes.insert("outbounce", QEasingCurve::OutBounce);
		easingCurveTypes.insert("inoutbounce", QEasingCurve::InOutBounce);
		easingCurveTypes.insert("outinbounce", QEasingCurve::OutInBounce);
	}

	QEasingCurve::Type getEasingCurveType(QString type)
	{
		return this->easingCurveTypes.value(type, QEasingCurve::Custom);
	}

private:
	QHash<QString, QEasingCurve::Type> easingCurveTypes;
};

class Mapping : public QObject
{
	Q_OBJECT
private:
	/*inline qreal customMapping(qreal progress)
	{
		if (this->mappingProvider != 0)
		{
			return this->mappingProvider->getMappingValue(mapping, progress);
		}
		else
		{
			return progress;
		}
	}*/

public:
	inline Mapping(QString mapping, MappingManager *mappingLoader, bool createTexture = false) :
		mapping(mapping),
		mappingProvider(mappingLoader)
	{
		if (easingCurveDictionary.getEasingCurveType(mapping) != QEasingCurve::Custom)
		{
			this->easingCurve.setType(easingCurveDictionary.getEasingCurveType(mapping));
			this->useEasingCurve = true;
		}
		else
		{
			this->useEasingCurve = false;

			if (this->mappingProvider->contains(mapping))
			{
				//TODO: use this this->easingCurve.setCustomType(&Mapping::customMapping);
			}
			else
			{
				this->easingCurve.setType(QEasingCurve::Linear);
				this->useEasingCurve = true;
			}
		}

		if (createTexture)
		{
			connect(mappingProvider, SIGNAL(resourceUpdated()), this, SLOT(update()));

			int resolution = 256;

			this->texture = new QOpenGLTexture(QOpenGLTexture::Target1D);
			this->texture->setWrapMode(QOpenGLTexture::ClampToEdge);
			this->texture->setSize(resolution);
			this->texture->setFormat(QOpenGLTexture::R32F);
			this->texture->allocateStorage();

			this->uploadTextureData(resolution);
		}
		else
		{
			this->texture = 0;
		}
	}

	inline void uploadTextureData(int resolution)
	{
		QVector<GLfloat> data;
		data.resize(resolution);

		for (int i = 0; i < resolution; i++)
		{
			data[i] = (GLfloat) this->mix(0.0, 1.0, (qreal) i / ((qreal) resolution - 1.0));
		}

		this->texture->setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, data.data());
		qDebug() << "updated texture" << resolution;
	}

	inline QOpenGLTexture *getTexture()
	{
		return this->texture;
	}

	template <class T> inline T mix(T p0, T p1, qreal t)
	{
		qreal _t = qMin(1.0, qMin(0.0, t));

		if (useEasingCurve)
		{
			_t = easingCurve.valueForProgress(t);
		}
		else
		{
			_t = this->mappingProvider->getMappingValue(mapping, t);
		}

		return (T) (p0 * (1.0 - _t) + p1 * _t);
	}

	static EasingCurveDictionary easingCurveDictionary;

	bool useEasingCurve;
	QEasingCurve easingCurve;
	QString mapping;

	MappingManager *mappingProvider;

	QOpenGLTexture *texture;

public slots:
	inline void update()
	{
		this->uploadTextureData(256);
	}
};

#endif // MAPPINGMANAGER_H
