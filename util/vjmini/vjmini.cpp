#include "vjmini.h"
#include <QtDebug>
#include <QEventLoop>
#include <QTimer>
#include <QPainter>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QDateTime>

VJMini::VJMini(QObject *parent) : QObject(parent)
{
	qDebug() << "VJ Mini initializing";

	this->onScreenSurface = 0;
	this->offScreenSurface = 0;

	nn = new NN(this);

	nn->init();

	//internal resource managers
	this->mappingManager = new MappingManager("mappings", this);
	this->shaderManager = new ShaderManager("shaders", this);
	this->textureProvider = new TextureProvider("textures", this);

	this->resourceManagers << this->mappingManager
						   << this->shaderManager;

	//external resource providers (managers and renderers)
	QList<ResourceProvider *> externalProviders;

	externalProviders << this->textureProvider
						 ;



	foreach (ResourceProvider *provider, externalProviders)
	{
		this->resourceManagers << provider;
		this->resourceRenderers << provider;
	}

	foreach (ResourceManager *provider, this->resourceManagers)
	{
		connect(provider, SIGNAL(resourceUpdated()), this, SLOT(getCurrentFrame()));
		connect(provider, SIGNAL(resourceUpdated()), this, SIGNAL(updated()));
	}

	this->scriptWatcher = 0;

	this->outputFbo = "output";
	this->tempFbo = "temp";
	this->pingFbo = "ping";
	this->pongFbo = "pong";

	qDebug() << "VJ initialized.";
}

VJMini::~VJMini()
{
	this->context->makeCurrent(this->onScreenSurface);
	nn->glDeleteTextures(1, &depthTexture);
}

void VJMini::reloadScriptFile(QString fileName)
{
	qDebug() << "ASKED TO RELOAD" << fileName << "...";
	QFileInfo info(fileName);

	qDebug() << "FILE SIZE = " << info.size();
	if (info.size() == 0)
		return;

	this->scriptWatcher->blockSignals(true);

	this->context->makeCurrent(this->onScreenSurface);

	if (fileName == this->resourcesFileName)
	{
		this->loadResources(fileName);
	}
	else if (fileName == this->configFileName)
	{
		this->loadConfig(fileName);
	}
	qDebug() << this->config.count() << "variables in the config";

	this->scriptWatcher->blockSignals(false);
}

QJsonDocument VJMini::loadJson(QString fileName)
{
	QFile file(fileName);

	int counter = 0;
	while (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Trying to open" << fileName << "...";
		counter++;

		QApplication::processEvents();

		QThread::msleep(100);

		if (counter > 50)
		{
			break;
		}
	}

	if (file.isOpen())
	{
		QJsonParseError error;
		QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

		if (doc.isNull())
		{
			qDebug() << "Json Error:" << fileName << "contains an error.";
			qDebug() << error.offset << ":" << error.errorString();
		}

		file.close();

		return doc;
	}
	else
	{
		return QJsonDocument();
	}
}

void VJMini::finalizeScriptFile(QString fileName)
{
	if (!this->scriptWatcher->files().contains(fileName))
	{
		this->scriptWatcher->addPath(fileName);
	}
	emit this->updated();
}

bool VJMini::loadResources(QString fileName)
{
	qDebug() << "+++ LOADING THE RESOURCES... +++";

	QJsonDocument doc = loadJson(fileName);

	if (doc.isObject())
	{
		QJsonObject object = doc.object();

		foreach (ResourceManager *manager, this->resourceManagers)
		{
			manager->clear();

			QString filePath = object[manager->getName()].toObject()["path"].toString();
			qDebug() << "loading" << manager->getName() << "from" << filePath << "...";
			manager->setFilePath(NNU::absolutePathRelativeTo(filePath, this->scriptFileName));
			QJsonObject resource = object[manager->getName()].toObject();
			QJsonObject files = resource["files"].toObject();

			QStringList defaultFileNames;
			if (resource["default files"].isArray())
			{
				QJsonArray defaultFiles = resource["default files"].toArray();
				foreach (QJsonValue defaultFile, defaultFiles)
				{
					defaultFileNames << defaultFile.toString();
				}
			}
			else if (resource["default files"].isString())
			{
				defaultFileNames << resource["default files"].toString();
			}


			foreach (QString key, files.keys())
			{
				qDebug() << "loading" << files[key].toString() << "as" << key;

				QStringList fileNames;

				if (files[key].isArray())
				{
					foreach (QJsonValue value, files[key].toArray())
					{
						fileNames << value.toString();
					}
				}
				else if (files[key].isObject())
				{
					qWarning() << "loading resource files specified as object is not implemented";
				}
				else if (files[key].isString())
				{
					fileNames << defaultFileNames;
					fileNames << files[key].toString();
				}

				manager->add(key, fileNames);

				/*//TODO: can this be done nicer?
				switch (provider->getAdditionalFiles().count())
				{
				default:
				case 0:
					provider->add(key, files[key].toString());
					break;
				case 1:
					provider->add(key, files[key].toString(), provider->getAdditionalFiles()[0]);
					break;
				case 2:
					provider->add(key, files[key].toString(), provider->getAdditionalFiles()[0], provider->getAdditionalFiles()[1]);
					break;
				}*/
			}
		}

		this->finalizeScriptFile(fileName);

		return true;
	}
	else
	{
		return false;
	}
}

//configuration file
bool VJMini::loadConfig(QString fileName)
{
	qDebug() << "+++ LOADING THE CONFIG... +++";

	QJsonDocument doc = loadJson(fileName);

	if (doc.isObject())
	{
		QJsonObject object = doc.object();

		this->config.clear();

		foreach (QString key, object.keys())
		{
			if (object[key].isArray())
			{
				QJsonArray a = object[key].toArray();

				bool isStringList = true;

				for (int i = 0; i < a.count(); i++)
				{
					if (!a[i].isString())
					{
						isStringList = false;
						break;
					}
				}

				if (isStringList)
				{
					this->config.insert(key, a.toVariantList());
				}
				else
				{
					switch (a.count())
					{
					default:
					case 1:
						this->config.insert(key, object[key].toVariant());
						break;
					case 2:
						this->config.insert(key, QVector2D(a[0].toVariant().toDouble(), a[1].toVariant().toDouble()));
						break;
					case 3:
						this->config.insert(key, QVector3D(a[0].toVariant().toDouble(), a[1].toVariant().toDouble(), a[2].toVariant().toDouble()));
						break;
					case 4:
						this->config.insert(key, QVector4D(a[0].toVariant().toDouble(), a[1].toVariant().toDouble(), a[2].toVariant().toDouble(), a[3].toVariant().toDouble()));
						break;
					case 16:
						this->config.insert(key, QMatrix4x4(
													a[0].toVariant().toDouble(), a[1].toVariant().toDouble(), a[2].toVariant().toDouble(), a[3].toVariant().toDouble(),
													a[4].toVariant().toDouble(), a[5].toVariant().toDouble(), a[6].toVariant().toDouble(), a[7].toVariant().toDouble(),
													a[8].toVariant().toDouble(), a[9].toVariant().toDouble(), a[10].toVariant().toDouble(), a[11].toVariant().toDouble(),
													a[12].toVariant().toDouble(), a[13].toVariant().toDouble(), a[14].toVariant().toDouble(), a[15].toVariant().toDouble()
												));
						break;
					}
				}
			}
			else
			{
				this->config.insert(key, object[key].toVariant());
			}
		}

		this->finalizeScriptFile(fileName);

		return true;
	}
	else
	{
		return false;
	}
}

bool VJMini::loadScriptFile(QString fileName)
{
	qDebug() << "+++ LOADING THE SCRIPT" << fileName << "... +++";

	QJsonDocument doc = loadJson(fileName);

	if (doc.isObject())
	{
		QJsonObject object = doc.object();

		if (object["files"].isObject())
		{
			QJsonObject filesObject = object["files"].toObject();

			resourcesFileName = NNU::absolutePathRelativeTo(filesObject["resources"].toString(), fileName);
			configFileName = NNU::absolutePathRelativeTo(filesObject["config"].toString(), fileName);

			this->blockSignals(true);

			//TODO: make the load functions check whether all the necessary load functions have been already called
			this->loadConfig(configFileName);
			this->loadResources(resourcesFileName);

			this->blockSignals(false);

			this->finalizeScriptFile(fileName);

			qDebug() << "script file finalized.";

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		qWarning() << "VJ Mini ERROR: Script file not found.";
		return false;
	}
}

bool VJMini::loadScript(QString fileName)
{
	this->scriptFileName = fileName;

	if (this->scriptWatcher != 0)
	{
		delete this->scriptWatcher;
	}

	this->scriptWatcher = new QFileSystemWatcher(this);
	connect(this->scriptWatcher, SIGNAL(fileChanged(QString)), this, SLOT(reloadScriptFile(QString)));

	if (this->loadScriptFile(fileName))
	{
		//outputSize has to be set in order for these to work
		//this->nn->fbos.clear();
		this->nn->addFbo(outputFbo);
		this->nn->addFbo(tempFbo);
		this->nn->addFbo(pingFbo);
		this->nn->addFbo(pongFbo);

		return true;
	}
	else
	{
		return false;
	}
}


QString msToTime(qint64 duration)
{
	qint64 ms = duration % 1000;
	qint64 s = (duration/1000) % 60;
	qint64 m = (duration/1000/60) % 60;
	qint64 h = (duration/1000/60/60) % 24;

	return QString("%1").arg(h, 2, 10, QChar('0')) + ":" +
		   QString("%1").arg(m, 2, 10, QChar('0')) + ":" +
		   QString("%1").arg(s, 2, 10, QChar('0')) + "." + QString::number(ms);
}
