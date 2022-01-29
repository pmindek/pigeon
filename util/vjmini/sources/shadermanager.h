#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <QObject>
#include "resourcemanager.h"

class ShaderManager : public ResourceManager
{
public:
	inline ShaderManager(QString name, QObject *parent = 0) : ResourceManager(name, parent)
	{
	}

	inline NNShader *shader(QString key)
	{
		return VPointer<NNShader>::toPointer(this->resources[key]);
	}
	inline NNShader *latest()
	{
		return VPointer<NNShader>::toPointer(this->resources.latest());
	}

	inline virtual void clear()
	{
		QHashIterator<QString, QVariant> i(this->resources.getResources());
		while (i.hasNext())
		{
			i.next();
			qDebug() << "deleting shader" << i.key();
			delete VPointer<NNShader>::toPointer(i.value());
		}

		ResourceManager::clear();
	}


	inline virtual bool add(QString key, QStringList fileNames)
	{
		qDebug() << "loading shader" << key << "from" << fileNames;

		QStringList fullFileNames;
		foreach (QString fileName, fileNames)
		{
			fullFileNames << this->filePath + fileName;
		}

		NNShader *shader = 0;
		switch (fullFileNames.count())
		{
		case 1:
			//compute
            shader = new NNShader(fullFileNames[0], this, true);
			break;
		case 2:
			//vs, fs
            shader = new NNShader(fullFileNames[0], fullFileNames[1], this, true);
			break;
		case 3:
			//vs, gs, fs
            shader = new NNShader(fullFileNames[0], fullFileNames[1], fullFileNames[2], this, true);
			break;
		case 5:
			//vs, tcs, tes, gs, fs
            shader = new NNShader(fullFileNames[0], fullFileNames[1], fullFileNames[2], fullFileNames[3], fullFileNames[4], this, true);
			break;

		default:
				return false;
		}

		shader->processFileChanges();
		QObject::connect(shader, SIGNAL(shaderReloaded(QString)), this, SLOT(resourceReloaded(QString)));

		if (!this->resources.add(key, VPointer<NNShader>::toVariant(shader)))
		{
			delete shader;
			return false;
		}
		else
		{
			return true;
		}
	}

public slots:
	inline void resourceReloaded(QString fileName)
	{
		//shader manager uses dynamic shaders, which reload themselves. here we just notify the change
		emit resourceUpdated();
	}

};

#endif // SHADERMANAGER_H
