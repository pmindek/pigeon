/* =======================================
 *               NN Utilities
 *                 v2018.01
 *
 *  Copyright (C) 2018  Nanographics GmbH
 *           All Rights Reserved
 * =======================================
 *
 * This set of classes and defines are the
 * part of the NN library. They provide
 * additional features not directly to the
 * NN library, not directly connected to
 * rendering.
 *
 */

#ifndef NNU_H
#define NNU_H

#include "nn_global.h"
#include <QtCore>
#include <QFileInfo>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QApplication>
#include <cmath>

#define NN_PI  3.14159265358979323846264338327950288419716939937510
#define NN_E   2.71828182845904523536028747135266249775724709369995
#define NN_PHI 1.61803398874989484820458683436563811772030917980576

#if QT_VERSION >= 0x050000
#define MOUSE_POS localPos
#else
#define MOUSE_POS posF
#endif

template <class T> class VPointer
{
public:
	static T* toPointer(QVariant v)
	{
		return  (T *) v.value<void *>();
	}

	static QVariant toVariant(T* pointer)
	{
		return qVariantFromValue((void *) pointer);
	}
};

class NN_API NNU
{
public:
	NNU();

	//file system
	static void persistentFileOpen(QFile *file, QIODevice::OpenMode openMode = QIODevice::ReadOnly, int tries = 50);
	static QString getCanonicalPath(QString fileName);
	static QString absolutePathRelativeTo(QString fileName, QString baseFile);

	//math
	static qreal random();
	static qreal randomOpen();

	static qreal mix(qreal a0, qreal a1, qreal x);
	static qreal mix(qreal a0, qreal a1, qreal a2, qreal a3, qreal x);
	static qreal mixCos(qreal a0, qreal a1, qreal x);
	static QPointF mix(QPointF p0, QPointF p1, qreal x);
	static QPointF mix(QPointF p0, QPointF p1, QPointF p2, QPointF p3, qreal x);
	static QPointF mixCos(QPointF p0, QPointF p1, qreal x);
	static QVector2D mix(QVector2D p0, QVector2D p1, qreal x);
	static QVector2D mix(QVector2D p0, QVector2D p1, QVector2D p2, QVector2D p3, qreal x);
	static QVector3D mix(QVector3D p0, QVector3D p1, qreal x);
	static QVector3D mix(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3, qreal x);
	static QVector4D mix(QVector4D p0, QVector4D p1, qreal x);
	static QVector4D mix(QVector4D p0, QVector4D p1, QVector4D p2, QVector4D p3, qreal x);
	static QMatrix4x4 mix(QMatrix4x4 p0, QMatrix4x4 p1, qreal x);
	static QMatrix4x4 mix(QMatrix4x4 p0, QMatrix4x4 p1, qreal xt, qreal xr);
	static QMatrix4x4 mix(QMatrix4x4 p0, QMatrix4x4 p1, QMatrix4x4 p2, QMatrix4x4 p3, qreal x);
	static QMatrix4x4 mix(QMatrix4x4 p0, QMatrix4x4 p1, QMatrix4x4 p2, QMatrix4x4 p3, qreal xt, qreal xr);

	static QVariant mix(QVariant a0, QVariant a1, qreal x);
	static QVariant mix(QVariant a0, QVariant a1, QVariant a2, QVariant a3, qreal x);

};

#endif // NNU_H
