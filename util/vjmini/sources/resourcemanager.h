#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <QtCore/QtGlobal>
#include <QVariant>
#include <QFileSystemWatcher>
#include <QtDebug>
#include "nn.h"
#include <cstdarg>

class ResourceManager : public QObject
{
Q_OBJECT
public:
	inline ResourceManager(QString name, QObject *parent) :
		QObject(parent),
		name(name)
	{
		watcher = 0;
		filePath = "";
	}

	inline virtual void clear()
	{
		if (this->watcher != 0)
			delete this->watcher;
		this->watcher = 0;

		this->resources.clear();
	}

	inline virtual bool add(QString key, QStringList fileNames) = 0;

	inline void setFilePath(QString prefix)
	{
		if (!prefix.endsWith("/") && !prefix.endsWith("\\"))
		{
			this->filePath = prefix + "/";
		}
		else
		{
			this->filePath = prefix;
		}
	}

	inline NNResource<QString, QVariant> *getResources()
	{
		return &resources;
	}

	inline QVariant get(QString key)
	{
		return this->resources.get(key);
	}

	inline QString getName()
	{
		return name;
	}

	inline bool contains(QString key)
	{
		return this->resources.contains(key);
	}

	inline void watchFiles(QString fileName)
	{
		QStringList fileNames;
		fileNames << fileName;

		this->watchFiles(fileNames);
	}

	inline void watchFiles(QStringList fileNames)
	{
		if (this->watcher == 0)
		{
			this->watcher = new QFileSystemWatcher(this);
			connect(this->watcher, SIGNAL(fileChanged(QString)), this, SLOT(resourceReloaded(QString)));
		}

		this->watcher->addPaths(fileNames);
	}

signals:
	void resourceUpdated();

public slots:
	inline virtual void resourceReloaded(QString)
	{
		qWarning() << "resourceReloaded(QString) is not implemented by" << this->name;
	}


protected:
	QString name;
	QString filePath;

	NNResource<QString, QVariant> resources;

	QFileSystemWatcher *watcher;
};

#endif // RESOURCEMANAGER_H
