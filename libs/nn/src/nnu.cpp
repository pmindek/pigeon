#include "nnu.h"

NNU::NNU()
{
}

//FILE SYSTEM

void NNU::persistentFileOpen(QFile *file, QFile::OpenMode openMode, int tries)
{
	int counter = 0;
	while (!file->open(openMode))
	{
		counter++;

		QApplication::processEvents();

		QThread::msleep(100);

		if (counter > tries)
		{
			break;
		}
	}
}

//returns canonical path to the file _fileName_. If the file doesn't exist, returns an empty string.
QString NNU::getCanonicalPath(QString fileName)
{
	QFileInfo fi(fileName);
	return fi.canonicalPath();
}

//if fileName is an absolute path, returns it. if it's relative, returns an absolute path as if fileName is relative to baseFile. if fileName is not an file, returns empty string.
QString NNU::absolutePathRelativeTo(QString fileName, QString baseFile)
{	
	QString basePath = NNU::getCanonicalPath(baseFile);

	if (!basePath.endsWith("/") && !basePath.isEmpty())
		basePath = basePath + "/";

	QString candidateFile = "";

	if (fileName.isEmpty())
	{
		return "";
	}

	QFileInfo fi(fileName);
	if (fi.isAbsolute())
	{
		candidateFile = fileName;
	}
	else
	{
		candidateFile = basePath + fileName;
	}

	QFileInfo cfi(candidateFile);
	if (!cfi.exists())
	{
		return "";
	}
	else
	{
		return candidateFile;
	}
}

//MATH

//returns a real random number from the interval [0,1]
qreal NNU::random()
{
	return (qreal) (rand() % RAND_MAX) / (qreal) (RAND_MAX - 1);
}

//returns a real random number from the interval [0,1)
qreal NNU::randomOpen()
{
	return (qreal) (rand() % RAND_MAX) / (qreal) RAND_MAX;
}

//linear interpolation between a0 and a1
qreal NNU::mix(qreal a0, qreal a1, qreal x)
{
	return a0 * (1.0 - x) + a1 * x;
}

//cosine interpolation between a0 and a1
qreal NNU::mixCos(qreal a0, qreal a1, qreal x)
{
	qreal f = (1.0 - cos(x * NN_PI)) * 0.5;
	return a0 * (1.0 - f) + a1 * f;
}

//cubic interpolation between a1 and a2
qreal NNU::mix(qreal a0, qreal a1, qreal a2, qreal a3, qreal x)
{
	qreal p,q,r,s;
	p = (a3 - a2) - (a0 - a1);
	q = (a0 - a1) - p;
	r = a2 - a0;
	s = a1;

	return p*x*x*x + q*x*x + r*x + s;
}

//linear interpolation between two points
QPointF NNU::mix(QPointF p0, QPointF p1, qreal x)
{
	return QPointF(
				mix(p0.x(), p1.x(), x),
				mix(p0.y(), p1.y(), x));
}

//cubic interpolation between two points
QPointF NNU::mix(QPointF p0, QPointF p1, QPointF p2, QPointF p3, qreal x)
{
	return QPointF(
				mix(p0.x(), p1.x(), p2.x(), p3.x(), x),
				mix(p0.y(), p1.y(), p2.y(), p3.y(), x));
}

//cosine interpolation between two points
QPointF NNU::mixCos(QPointF p0, QPointF p1, qreal x)
{
	return QPointF(
				mix(p0.x(), p1.x(), x),
				mixCos(p0.y(), p1.y(), x));
}

//linear interpolation between two 2D vectors
QVector2D NNU::mix(QVector2D p0, QVector2D p1, qreal x)
{
	return QVector2D(
				mix(p0.x(), p1.x(), x),
				mix(p0.y(), p1.y(), x));
}

//cubic interpolation between two 2D vectors
QVector2D NNU::mix(QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3, qreal x)
{
	return QVector2D(
				mix(p0.x(), p1.x(), p2.x(), p3.x(), x),
				mix(p0.y(), p1.y(), p2.y(), p3.y(), x));
}

//linear interpolation between two 3D vectors
QVector3D NNU::mix(QVector3D p0, QVector3D p1, qreal x)
{
	return QVector3D(
				mix(p0.x(), p1.x(), x),
				mix(p0.y(), p1.y(), x),
				mix(p0.z(), p1.z(), x));
}

//cubic interpolation between two 3D vectors
QVector3D NNU::mix(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, qreal x)
{
	return QVector3D(
				mix(p0.x(), p1.x(), p2.x(), p3.x(), x),
				mix(p0.y(), p1.y(), p2.y(), p3.y(), x),
				mix(p0.z(), p1.z(), p2.z(), p3.z(), x));
}

//linear interpolation between two 4D vectors
QVector4D NNU::mix(QVector4D p0, QVector4D p1, qreal x)
{
	return QVector4D(
				mix(p0.x(), p1.x(), x),
				mix(p0.y(), p1.y(), x),
				mix(p0.z(), p1.z(), x),
				mix(p0.w(), p1.w(), x));
}

//cubic interpolation between two 4D vectors
QVector4D NNU::mix(QVector4D p0, QVector4D p1, QVector4D p2, QVector4D p3, qreal x)
{
	return QVector4D(
				mix(p0.x(), p1.x(), p2.x(), p3.x(), x),
				mix(p0.y(), p1.y(), p2.y(), p3.y(), x),
				mix(p0.z(), p1.z(), p2.z(), p3.z(), x),
				mix(p0.w(), p1.w(), p2.w(), p3.w(), x));
}

QMatrix4x4 NNU::mix(QMatrix4x4 p0, QMatrix4x4 p1, qreal x)
{
	return NNU::mix(p0, p1, x, x);
}

QMatrix4x4 NNU::mix(QMatrix4x4 p0, QMatrix4x4 p1, qreal xt, qreal xr)
{
	float v[9];
	v[0] = p0.data()[0];
	v[1] = p0.data()[4];
	v[2] = p0.data()[8];
	v[3] = p0.data()[1];
	v[4] = p0.data()[5];
	v[5] = p0.data()[9];
	v[6] = p0.data()[2];
	v[7] = p0.data()[6];
	v[8] = p0.data()[10];
	QMatrix3x3 rotation0(v);
	QVector3D pan0(p0.data()[12], p0.data()[13], p0.data()[14]);

	v[0] = p1.data()[0];
	v[1] = p1.data()[4];
	v[2] = p1.data()[8];
	v[3] = p1.data()[1];
	v[4] = p1.data()[5];
	v[5] = p1.data()[9];
	v[6] = p1.data()[2];
	v[7] = p1.data()[6];
	v[8] = p1.data()[10];
	QMatrix3x3 rotation1(v);
	QVector3D pan1(p1.data()[12], p1.data()[13], p1.data()[14]);

	QQuaternion r =
	QQuaternion::slerp(QQuaternion::fromRotationMatrix(rotation0),
					   QQuaternion::fromRotationMatrix(rotation1),
					   xr);
	QVector3D p = NNU::mix(pan0, pan1, xt);

	QMatrix4x4 result;

	result.setToIdentity();
	result.translate(p);
	result.rotate(r);

	return result;
}

QMatrix4x4 NNU::mix(QMatrix4x4 p0, QMatrix4x4 p1, QMatrix4x4 p2, QMatrix4x4 p3, qreal x)
{
	return NNU::mix(p0, p1, p2, p3, x, x);
}

QMatrix4x4 NNU::mix(QMatrix4x4 p0, QMatrix4x4 p1, QMatrix4x4 p2, QMatrix4x4 p3, qreal xt, qreal xr)
{
	QVector3D pan0(p0.data()[12], p0.data()[13], p0.data()[14]);

	float v[9];
	v[0] = p1.data()[0];
	v[1] = p1.data()[4];
	v[2] = p1.data()[8];
	v[3] = p1.data()[1];
	v[4] = p1.data()[5];
	v[5] = p1.data()[9];
	v[6] = p1.data()[2];
	v[7] = p1.data()[6];
	v[8] = p1.data()[10];
	QMatrix3x3 rotation1(v);
	QVector3D pan1(p1.data()[12], p1.data()[13], p1.data()[14]);

	v[0] = p2.data()[0];
	v[1] = p2.data()[4];
	v[2] = p2.data()[8];
	v[3] = p2.data()[1];
	v[4] = p2.data()[5];
	v[5] = p2.data()[9];
	v[6] = p2.data()[2];
	v[7] = p2.data()[6];
	v[8] = p2.data()[10];
	QMatrix3x3 rotation2(v);
	QVector3D pan2(p2.data()[12], p2.data()[13], p2.data()[14]);

	QVector3D pan3(p3.data()[12], p3.data()[13], p3.data()[14]);

	QQuaternion r =
	QQuaternion::slerp(QQuaternion::fromRotationMatrix(rotation1),
					   QQuaternion::fromRotationMatrix(rotation2),
					   xr);
	QVector3D p = NNU::mix(pan0, pan1, pan2, pan3, xt);

	QMatrix4x4 result;

	result.setToIdentity();
	result.translate(p);
	result.rotate(r);

	return result;
}

QVariant NNU::mix(QVariant a0, QVariant a1, qreal x)
{
	/*if (a0.type() != a1.type())
		return QVariant();*/

	switch (a0.type())
	{
	default:
	case QMetaType::Double:
	case QMetaType::Float:
	case QMetaType::Int:
	case QMetaType::UInt:
		return QVariant(NNU::mix(a0.toDouble(), a1.toDouble(), x));
		break;
	case QMetaType::QVector2D:
		return QVariant(NNU::mix(qvariant_cast<QVector2D> (a0), qvariant_cast<QVector2D> (a1), x));
		break;
	case QMetaType::QVector3D:
		return QVariant(NNU::mix(qvariant_cast<QVector3D> (a0), qvariant_cast<QVector3D> (a1), x));
		break;
	case QMetaType::QVector4D:
		return QVariant(NNU::mix(qvariant_cast<QVector4D> (a0), qvariant_cast<QVector4D> (a1), x));
		break;
	case QMetaType::QMatrix4x4:
		return QVariant(NNU::mix(qvariant_cast<QMatrix4x4> (a0), qvariant_cast<QMatrix4x4> (a1), x));
		break;
	}
}

QVariant NNU::mix(QVariant a0, QVariant a1, QVariant a2, QVariant a3, qreal x)
{/*
	if (a0.type() != a1.type())
		return QVariant();*/

	switch (a0.type())
	{
	default:
	case QMetaType::Double:
	case QMetaType::Float:
	case QMetaType::Int:
	case QMetaType::UInt:
		return QVariant(NNU::mix(a0.toDouble(), a1.toDouble(), a2.toDouble(), a3.toDouble(), x));
		break;
	case QMetaType::QVector2D:
		return QVariant(NNU::mix(qvariant_cast<QVector2D> (a0), qvariant_cast<QVector2D> (a1), qvariant_cast<QVector2D> (a2), qvariant_cast<QVector2D> (a3), x));
		break;
	case QMetaType::QVector3D:
		return QVariant(NNU::mix(qvariant_cast<QVector3D> (a0), qvariant_cast<QVector3D> (a1), qvariant_cast<QVector3D> (a2), qvariant_cast<QVector3D> (a3), x));
		break;
	case QMetaType::QVector4D:
		return QVariant(NNU::mix(qvariant_cast<QVector4D> (a0), qvariant_cast<QVector4D> (a1), qvariant_cast<QVector4D> (a2), qvariant_cast<QVector4D> (a3), x));
		break;
	case QMetaType::QMatrix4x4:
		return QVariant(NNU::mix(qvariant_cast<QMatrix4x4> (a0), qvariant_cast<QMatrix4x4> (a1), qvariant_cast<QMatrix4x4> (a2), qvariant_cast<QMatrix4x4> (a3), x));
		break;
	}
}
