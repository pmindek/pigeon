#ifndef ONE_H
#define ONE_H

#include <QtOpenGL/QGL>
#include <QOpenGLFunctions_2_0>
#include "moveable.h"

class One : public Moveable
{
public:
	One(QOpenGLFunctions_2_0 *gl);
	void display(GLfloat offset, GLuint texture);

	void setRight(bool right);
	void setLastX(GLfloat lastX);
	void setLastY(GLfloat lastY);
	void setLastAngle(GLfloat lastAngle);
	GLfloat getLastX();
	GLfloat getLastY();
	bool onFeet();

	void setImmortal(bool immortal);
	bool isImmortal();

	inline bool isRight()
	{
		return right;
	}

	inline void setPeck(qreal peck)
	{
		this->peck = peck;
	}
	inline qreal getPeck() const
	{
		return this->peck;
	}


	bool immortal;


private:
	GLuint textureId;
	GLfloat lastX, lastY, lastAngle;
	bool right;

	qreal peck;


	QOpenGLFunctions_2_0 *gl;
};

#endif // ONE_H
