#ifndef RESOURCEPROVIDER_H
#define RESOURCEPROVIDER_H

#include "resourcemanager.h"
#include "resourcerenderer.h"

class ResourceProvider : public ResourceManager, public ResourceRenderer
{
public:
	inline ResourceProvider(QString name, QObject *parent = 0) : ResourceManager(name, parent), ResourceRenderer(name)
	{

	}
};

#endif // RESOURCEPROVIDER_H
