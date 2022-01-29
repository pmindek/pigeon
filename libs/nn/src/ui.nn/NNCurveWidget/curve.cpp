#include "curve.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QtDebug>

int Curve::counter = 0;

Curve::Curve(QObject *parent) : QObject(parent)
{	
	CurvePoint p;
	p.setRelativePosition(0.0, 0.0);
	points << p;
	p.setRelativePosition(1.0, 1.0);
	points << p;
	curveType = 2;
	drawControlPoints = true;
	color = QColor(255, 255, 255, 255);
	fileName = "";
	resolution = 2048;
	this->modified = false;

	this->id = Curve::counter;

	Curve::counter++;
}

QList<qreal> Curve::getCurve(int resolution)
{
	CurvePoint::savePlaneSize();
	CurvePoint::setPlaneSize(resolution - 1, resolution - 1);

	QList<qreal> curve;
	curve.clear();

	this->precalc();

	for (int i = 0; i < resolution; i++)
	{
		curve << 1.0 - this->interpolate((double) i, (double) CurvePoint::getPlaneHeight()) / (double) CurvePoint::getPlaneHeight();
	}

	CurvePoint::restorePlaneSize();

	return curve;
}

bool Curve::save()
{
	return this->save(this->fileName);
}

bool Curve::save(QString fileName)
{
	QJsonDocument document;
	QJsonObject object;
	QJsonArray curve;
	QJsonArray curveControlPoints;
	QJsonArray curveColor;


	QList<qreal> curveData = this->getCurve(this->resolution);

	for (int i = 0; i < this->resolution; i++)
	{
		curve.append(QJsonValue(curveData[i]));
	}

	for (int i = 0; i < this->points.count(); i++)
	{
		QJsonArray point;
		point.append(QJsonValue(this->points[i].getRelativeX()));
		point.append(QJsonValue(this->points[i].getRelativeY()));
		curveControlPoints.append(QJsonValue(point));
	}

	curveColor.append((double) this->color.red() / 255.0);
	curveColor.append((double) this->color.green() / 255.0);
	curveColor.append((double) this->color.blue() / 255.0);

	//object->insert("color", QJsonValue(this->color));
	object.insert("resolution", QJsonValue(this->resolution));
	object.insert("curve", curve);
	object.insert("control points", curveControlPoints);
	object.insert("curve type", this->getCurveTypeString());
	object.insert("color", curveColor);

	document.setObject(object);


	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		//qWarning("Could not write the file " + fileName + ".");
		return false;
	}

	file.write(document.toJson());
	file.close();

	this->fileName = fileName;
	this->setModified(false);
	return true;
}

bool Curve::load(QString fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		return false;
	}

	QJsonDocument document = QJsonDocument::fromJson(file.readAll());

	points.clear();
	CurvePoint p;
	foreach (QJsonValue value, document.object()["control points"].toArray())
	{
		p.setRelativePosition(value.toArray()[0].toDouble(), value.toArray()[1].toDouble());
		points << p;
	}

	this->color = QColor((document.object()["color"].toArray()[0].toDouble() * 255.0),
						 (document.object()["color"].toArray()[1].toDouble() * 255.0),
						 (document.object()["color"].toArray()[2].toDouble() * 255.0),
						 255);

	this->resolution = document.object()["resolution"].toDouble();

	this->curveType = Curve::getCurveTypeFromString(document.object()["curve type"].toString());


	QFileInfo fi(fileName);
	this->fileName = fi.canonicalFilePath();

	this->setModified(false);
}

QList<CurvePoint> *Curve::getControlPoints()
{
	return &points;
}

QColor Curve::getColor()
{
	return this->color;
}

void Curve::setColor(QColor color)
{
	this->color = color;

	this->setModified(true);
}

int Curve::getCurveType()
{
	return curveType;
}

void Curve::setCurveType(int curveType)
{
	this->curveType = curveType;

	this->setModified(true);
}

bool Curve::getDrawControlPoints()
{
	return this->drawControlPoints;
}

void Curve::setDrawControlPoints(bool drawControlPoints)
{
	this->drawControlPoints = drawControlPoints;
}

void Curve::reset()
{
	points.clear();
	CurvePoint p;

	p.setRelativePosition(0.0, 0.0);
	points << p;
	p.setRelativePosition(1.0, 1.0);
	points << p;

	this->setModified(true);
}

void Curve::solveTridiag(QVector<double> &sub, QVector<double> &diag, QVector<double> &sup, QVector<double> &b, int n)
/*	solve linear system with tridiagonal n by n matrix a
	using Gaussian elimination *without* pivoting
	where   a(i,i-1) = sub[i]  for 2<=i<=n
			a(i,i)   = diag[i] for 1<=i<=n
			a(i,i+1) = sup[i]  for 1<=i<=n-1
	(the values sub[1], sup[n] are ignored)
	right hand side vector b[1:n] is overwritten with solution
	NOTE: 1...n is used in all arrays, 0 is unused
*/
{
	for(int i = 2; i <= n; i++)
	{
		sub[i] = sub[i] / diag[i - 1];
		diag[i] = diag[i] - sub[i] * sup[i - 1];
		b[i] = b[i] - sub[i] * b[i - 1];
	}
	b[n] = b[n] / diag[n];
	for(int i = n-1; i >= 1; i--)
	{
		b[i] = (b[i] - sup[i] * b[i + 1]) / diag[i];
	}
}

void Curve::precalc()
{
	h.clear();
	h.resize(this->getControlPoints()->count());
	h[0] = 0;

	a.clear();
	a.resize(this->getControlPoints()->count());
	a[0] = 0;
	a[this->getControlPoints()->count() - 1] = 0;

	for (int i = 1; i < this->getControlPoints()->count(); i++)
	{
		h[i] = (this->getControlPoints()->at(i).getAbsoluteXd() - this->getControlPoints()->at(i - 1).getAbsoluteXd());
	}

	if (this->getControlPoints()->count() > 2)
	{
		QVector<double> sub;
		sub.resize(this->getControlPoints()->count() - 1);
		sub[0] = 0;
		QVector<double> diag;
		diag.resize(this->getControlPoints()->count() - 1);
		diag[0] = 0;
		QVector<double> sup;
		sup.resize(this->getControlPoints()->count() - 1);
		sup[0] = 0;

		for (int i = 1; i < this->getControlPoints()->count() - 1; i++)
		{
			diag[i] = (h[i] + h[i + 1]) / 3;
			sup[i] = h[i + 1] / 6;
			sub[i] = h[i] / 6;
			a[i] = (this->getControlPoints()->at(i + 1).getAbsoluteYd() - this->getControlPoints()->at(i).getAbsoluteYd()) / h[i + 1] -
				   (this->getControlPoints()->at(i).getAbsoluteYd() - this->getControlPoints()->at(i - 1).getAbsoluteYd()) / h[i];
		}

		solveTridiag(sub, diag, sup, a, this->getControlPoints()->count() - 2);
	}
}

double Curve::interpolate(double x, double max)
{
	if (this->getControlPoints()->count() == 0)
	{
		return 0.0;
	}

	if (this->getControlPoints()->count() == 1)
	{
		return this->getControlPoints()->first().getAbsoluteYd();
	}

	if (x <= this->getControlPoints()->first().getAbsoluteX())
	{
		return this->getControlPoints()->first().getAbsoluteYd();
	}

	if (x >= this->getControlPoints()->last().getAbsoluteX())
	{
		return this->getControlPoints()->last().getAbsoluteYd();
	}

	double t1, t2;
	double y = 0;
	int i = 0;

	switch (this->getCurveType())
	{
	case 0:
	//LINEAR INTERPOLATION
		for (int k = 1; k < this->getControlPoints()->count(); k++)
		{
			if (x > this->getControlPoints()->at(k - 1).getAbsoluteXd() && x <= this->getControlPoints()->at(k).getAbsoluteXd())
			{
				i = k;
				break;
			}
		}

		y = this->getControlPoints()->at(i - 1).getAbsoluteYd() +
			(this->getControlPoints()->at(i).getAbsoluteYd() - this->getControlPoints()->at(i - 1).getAbsoluteYd()) /
			(this->getControlPoints()->at(i).getAbsoluteXd() - this->getControlPoints()->at(i - 1).getAbsoluteXd()) *
			(x - this->getControlPoints()->at(i - 1).getAbsoluteXd());

		break;

	case 1:
	//LAGRANGE INTERPOLATING POLYNOMIAL
		for (int j = 0; j < this->getControlPoints()->length(); j++)
		{
			t1 = this->getControlPoints()->at(j).getAbsoluteYd();
			for (int k = 0; k < this->getControlPoints()->length(); k++)
			{
				if (k == j)
					continue;
				t1 *= ((x - this->getControlPoints()->at(k).getAbsoluteXd()) / (this->getControlPoints()->at(j).getAbsoluteXd() - this->getControlPoints()->at(k).getAbsoluteXd()));
			}

			y += t1;
		}

	break;

	case 2:
	//NATURAL CUBIC SPLINE
		for (int k = 1; k < this->getControlPoints()->count(); k++)
		{
			if (x > this->getControlPoints()->at(k - 1).getAbsoluteXd() && x <= this->getControlPoints()->at(k).getAbsoluteXd())
			{
				i = k;
				break;
			}
		}

		t1 = x - this->getControlPoints()->at(i - 1).getAbsoluteXd();
		t2 = h[i] - t1;
		y = (
			 (-a[i - 1] / 6 * (t2 + h[i]) * t1 + this->getControlPoints()->at(i - 1).getAbsoluteYd()) * t2 +
			 (-a[i] / 6 * (t1 + h[i]) * t2 + this->getControlPoints()->at(i).getAbsoluteYd()) * t1
			) / h[i];

	break;

	default:
	break;
	}

	if (y < 0) y = 0;
	if (y > max) y = max;

	return y;
}
