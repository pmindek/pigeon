#ifndef PARAMETER_H
#define PARAMETER_H

#include "mappingmanager.h"

class Parameter
{
public:
	inline Parameter()
	{
		this->from = 0;
		this->to = 0;
		this->mappingBegin = 0;
		this->mappingEnd = 1;
		this->mapping = 0;
		this->type = Numerical;
		this->modifier = Default;
	}

	inline ~Parameter()
	{
		if (this->mapping != 0)
			delete this->mapping;
	}

	inline bool isAutomated()
	{
		return this->mapping != 0;
	}

	inline void setAutomated(Mapping *mapping)
	{
		this->mapping = mapping;
	}

	inline void setStatic()
	{
		this->mapping = 0;
	}

	inline int getModifier()
	{
		return this->modifier;
	}

	inline void setModifier(int modifier)
	{
		this->modifier = modifier;
	}

	inline QVariant getValue(qreal zo)
	{
		if (this->mapping != 0)
		{
			if (zo < mappingBegin)
			{
				return from;
			}
			else if (zo > mappingEnd)
			{
				return to;
			}
			else
			{
				qreal _zo = (zo - this->mappingBegin) / (this->mappingEnd - this->mappingBegin);

				switch (from.type())
				{
				case QMetaType::Double:
				case QMetaType::Float:
					return QVariant(mapping->mix(from.toDouble(), to.toDouble(), _zo));
					break;
				case QMetaType::Int:
					return QVariant(mapping->mix(from.toInt(), to.toInt(), _zo));
					break;
				case QMetaType::UInt:
					return QVariant(mapping->mix(from.toUInt(), to.toUInt(), _zo));
					break;
				case QMetaType::QVector2D:
					return QVariant(mapping->mix(qvariant_cast<QVector2D> (from), qvariant_cast<QVector2D> (to), _zo));
					break;
				case QMetaType::QVector3D:
					return QVariant(mapping->mix(qvariant_cast<QVector3D> (from), qvariant_cast<QVector3D> (to), _zo));
					break;
				case QMetaType::QVector4D:
					return QVariant(mapping->mix(qvariant_cast<QVector4D> (from), qvariant_cast<QVector4D> (to), _zo));
					break;
				}
			}
		}
		else
		{
			return from;
		}
	}

	inline bool hasTexture1D()
	{
		if (this->mapping == 0)
			return false;

		if (this->mapping->getTexture() == 0)
			return false;

		return true;
	}

	//relative begin and end of the value mapping within the parameter lifetime (see getValue(qreal) for details)
	qreal mappingBegin;
	qreal mappingEnd;

	QVariant from;
	QVariant to;
	Mapping *mapping;

	int modifier;

	enum ParameterType {Numerical = 0, Texture1D, Texture2D, Texture3D};
	enum ParameterModifier {DepthBuffer = -1, Default = 0};
	ParameterType type;
};


#endif // PARAMETER_H
