#include "curvepoint.h"

double CurvePoint::planeWidth;
double CurvePoint::planeHeight;
double CurvePoint::planeWidthTemp;
double CurvePoint::planeHeightTemp;
double CurvePoint::xFrom;
double CurvePoint::xTo;

CurvePoint::CurvePoint(double x, double y)
{
	this->x = x;
	this->y = y;
}

CurvePoint::~CurvePoint()
{
}

int CurvePoint::getPlaneHeight()
{
	return CurvePoint::planeHeight;
}

void CurvePoint::setPlaneSize(QSize size)
{
	CurvePoint::setPlaneSize(size.width(), size.height());
}

void CurvePoint::setPlaneSize(int planeWidth, int planeHeight)
{
	CurvePoint::planeWidth = (double) planeWidth;
	CurvePoint::planeHeight = (double) planeHeight;
}

void CurvePoint::savePlaneSize()
{
	CurvePoint::planeWidthTemp = CurvePoint::planeWidth;
	CurvePoint::planeHeightTemp = CurvePoint::planeHeight;
}

void CurvePoint::restorePlaneSize()
{
	CurvePoint::planeWidth = CurvePoint::planeWidthTemp;
	CurvePoint::planeHeight = CurvePoint::planeHeightTemp;
}

void CurvePoint::setRelativePosition(double x, double y)
{
	this->x = x;
	this->y = y;
}

void CurvePoint::setRelativeX(double x)
{
	this->x = x;
}

void CurvePoint::setRelativeY(double y)
{
	this->y = y;
}

void CurvePoint::setAbsolutePosition(int absoluteX, int absoluteY, bool axis)
{
	if (absoluteX < 0)
	{
		absoluteX = 0;
	}
	if (absoluteX > planeWidth)
	{
		absoluteX = planeWidth;
	}
	if (absoluteY < 0)
	{
		absoluteY = 0;
	}
	if (absoluteY > planeHeight)
	{
		absoluteY = planeHeight;
	}

	this->x = this->xFrom + ((double) absoluteX / CurvePoint::planeWidth) * (this->xTo - this->xFrom);
	this->y = 1.0 - (double) absoluteY / CurvePoint::planeHeight;

	if (axis)
	{
		this->y = x;
	}
}

double CurvePoint::getRelativeX() const
{
	return x;
}

double CurvePoint::getRelativeY() const
{
	return y;
}

int CurvePoint::getAbsoluteX() const
{
	return (int) this->getAbsoluteXd();
}

int CurvePoint::getAbsoluteY() const
{
	return (int) this->getAbsoluteYd();
}

double CurvePoint::getAbsoluteXd() const
{
	double _x = (x - this->xFrom) / (this->xTo - this->xFrom);

	return (_x * CurvePoint::planeWidth);
}

double CurvePoint::getAbsoluteYd() const
{
	return ((1.0 - y) * CurvePoint::planeHeight);
}

QPoint CurvePoint::getAbsolutePos() const
{
	return QPoint(this->getAbsoluteX(), this->getAbsoluteY());
}

void CurvePoint::multiply(double tx, double ty)
{
	this->x *= tx;
	this->y *= ty;
}

void CurvePoint::add(double tx, double ty)
{
	this->x += tx;
	this->y += ty;
}

void CurvePoint::setXRange(double from, double to)
{
	CurvePoint::xFrom = from;
	CurvePoint::xTo = to;
}
