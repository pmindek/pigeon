#ifndef CURVEPOINT_H
#define CURVEPOINT_H

#include <QPoint>
#include <QSize>

class CurvePoint
{
public:
	CurvePoint(double x = 0, double y = 0);
	~CurvePoint();

	static int getPlaneHeight();
	static void setPlaneSize(QSize size);
	static void setPlaneSize(int planeWidth, int planeHeight);
	static void savePlaneSize();
	static void restorePlaneSize();
	static void setXRange(double from, double to);

	void setRelativePosition(double x, double y);
	void setRelativeX(double x);
	void setRelativeY(double y);
	void setAbsolutePosition(int absoluteX, int absoluteY, bool axis = false);
	double getRelativeX() const;
	double getRelativeY() const;
	int getAbsoluteX() const;
	int getAbsoluteY() const;
	double getAbsoluteXd() const;
	double getAbsoluteYd() const;

	QPoint getAbsolutePos() const;

	void multiply(double tx, double ty);
	void add(double tx, double ty);
private:
	double x, y;

	static double xFrom, xTo;

	static double planeWidth;
	static double planeHeight;

	static double planeWidthTemp;
	static double planeHeightTemp;
};

#endif // CURVEPOINT_H
