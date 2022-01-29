#ifndef NNRESOURCE_H
#define NNRESOURCE_H

#include "nn_global.h"
#include <QHash>
#include <QtDebug>

/*
 */

template <class K, class T> class NNResource
{
public:
	inline NNResource()
	{
		_latest = T();
	}

	inline void unite(NNResource<K, T> r)
	{
		this->resources.unite(r.getResources());
	}

	inline void replace(K key, T resource)
	{
		this->resources.insert(key, resource);
	}

	inline bool add(K key, T resource)
	{
		if (!this->resources.contains(key))
		{
			this->resources.insert(key, resource);
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool set(QHash<K, T> resources)
	{
		this->resources.clear();
		this->resources = QHash<K, T>(resources);

		if (this->resources.count() == resources.count())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool add(K key, T resource, QString fileName)
	{
		if (this->add(key, resource))
		{
			keyLookUp.insert(fileName, key);
			return true;
		}
		else
		{
			return false;
		}
	}

	inline bool add(K key, T resource, QStringList fileNames)
	{
		if (this->add(key, resource))
		{
			foreach (QString fileName, fileNames)
			{
				keyLookUp.insert(fileName, key);
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	inline K getKeyFromFileName(K fileName)
	{
		return this->keyLookUp.value(fileName);
	}

	inline bool contains(K key)
	{
		return this->resources.contains(key);
	}

	inline T get(K key)
	{
		if (!resources.contains(key))
		{
			qDebug() << "NNResource::get():" << key << "key does not exist.";
		}


		this->_latest = resources.value(key, T());
		this->_latestKey = key;
		return this->_latest;
	}

	inline T operator[](K key)
	{
		return this->get(key);
	}

	inline T latest()
	{
		return _latest;
	}

	inline int count()
	{
		return this->resources.count();
	}

	inline void clear()
	{
		this->resources.clear();
		this->keyLookUp.clear();
		this->_latest = T();
	}

	inline QHash<K, T> getResources()
	{
		return this->resources;
	}

private:
	QHash<K, T> resources;
	QHash<QString, K> keyLookUp;
	T _latest;
	QString _latestKey;
};

#endif // NNRESOURCE_H
