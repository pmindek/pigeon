#ifndef CURVE_H
#define CURVE_H

#include <QObject>
#include <QColor>
#include <QVector>
#include <QFileInfo>
#include "curvepoint.h"

class Curve : public QObject
{
	Q_OBJECT
public:
	explicit Curve(QObject *parent = nullptr);
	QList<CurvePoint> *getControlPoints();
	QColor getColor();
	void setColor(QColor color);
	void reset();

	int getCurveType();
	void setCurveType(int curveType);

	bool getDrawControlPoints();
	void setDrawControlPoints(bool drawControlPoints);

	//function sets the modified flag, and emits curveChanged signal if the flag actually changed
	inline void setModified(bool modified)
	{
		if (this->modified != modified)
		{
			this->modified = modified;
			emit curveChanged();
		}
	}

	inline void setAutosave(bool autosave)
	{
		this->autosave = autosave;
	}

	inline bool isAutosave()
	{
		return this->autosave;
	}
	inline bool isModified()
	{
		return this->modified;
	}
	inline bool isSaved()
	{
		return !this->fileName.isEmpty();
	}
	inline QString getFileName(bool displayName = true)
	{
		if (this->fileName.isEmpty())
		{
			QString untitled = tr("untitled");
			QString number = this->id == 0 ? "" : (" " + QString::number(this->id));
			return displayName ? "(" + untitled + number + ")" : untitled + number;
		}
		else
		{
			QFileInfo fi(this->fileName);
			return fi.fileName();
		}
	}

	bool save();
	bool save(QString fileName);
	bool load(QString fileName);

	QList<qreal> getCurve(int resolution);
	void solveTridiag(QVector<double> &sub, QVector<double> &diag, QVector<double> &sup, QVector<double> &b, int n);
	void precalc();
	double interpolate(double x, double max);

	static inline int getCurveTypesCount()
	{
		return 3;
	}

	inline QString getCurveTypeString()
	{
		return Curve::getCurveTypeStringByIndex(this->curveType);
	}

	static inline QString getCurveTypeStringByIndex(int index)
	{
		switch (index)
		{
		case 0:
			return "Linear";
			break;
		case 1:
			return "Polynomial";
			break;
		default:
		case 2:
			return "Cubic Spline";
			break;
		}
	}

	static inline int getCurveTypeFromString(QString string)
	{
		QString s = string.toLower().trimmed();
		if (s == "linear")
		{
			return 0;
		}
		else if (s == "polynomial")
		{
			return 1;
		}
		else if (s == "cubic spline")
		{
			return 2;
		}
	}

private:
	static int counter;
	int id;
	bool drawControlPoints;
	int curveType;
	QList<CurvePoint> points;
	QColor color;
	QString fileName;
	int resolution;
	bool modified;
	bool autosave;

	//spline interpolation
	QVector<double> h;
	QVector<double> a;

signals:
	void curveChanged();

public slots:
};

#endif // CURVE_H
