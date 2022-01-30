#include "one.h"
#include <QtDebug>

One::One(QOpenGLFunctions_2_0 *gl)
{
	this->gl = gl;
	this->x = 0;
	this->y = 0;
	this->lastX = 0;
	this->lastY = 0;
	this->right = true;
	this->immortal = false;
	this->angle = 0.0;
	this->peck = 0.0;
	//this->y = 1.0;
}

void One::display(GLfloat offset, GLuint texture, bool isMan)
{
	size = 4.0;
	alpha = 1.0;

	gl->glEnable(GL_TEXTURE_2D);

	gl->glPushMatrix();
	gl->glTranslatef(x + offset, 10 - y, 0.0);
	gl->glRotatef(angle, 0.0, 0.0, 1.0);

	if (this->immortal)
	{
		gl->glColor4f(1.0, 0.95, 0.4, 1.0);
	}
	else
	{
		gl->glColor4f(1.0, 1.0, 1.0, 1.0);
	}

	gl->glColor4f(1.0, 1.0, 1.0, 1.0);

	qreal voffset = isMan ? -0.3 : -0.45;

	if (right)
	{
		if (texture > 0)
		{
			gl->glBindTexture(GL_TEXTURE_2D, texture);
			gl->glBegin(GL_QUADS);
				gl->glTexCoord2f(0.0, 1.0); gl->glVertex2f(-size / 2.0, - size * (isMan ? 2.0 : 1.0) + voffset);
				gl->glTexCoord2f(1.0, 1.0); gl->glVertex2f(+size / 2.0, - size * (isMan ? 2.0 : 1.0) + voffset);
				gl->glTexCoord2f(1.0, 0.0); gl->glVertex2f(+size / 2.0, 0.0 + voffset);
				gl->glTexCoord2f(0.0, 0.0); gl->glVertex2f(-size / 2.0, 0.0 + voffset);
			gl->glEnd();
		}
	}
	else
	{
		if (texture > 0)
		{
			gl->glBindTexture(GL_TEXTURE_2D, texture);
			gl->glBegin(GL_QUADS);
				gl->glTexCoord2f(1.0, 1.0); gl->glVertex2f(-size / 2.0, - size * (isMan ? 2.0 : 1.0) + voffset);
				gl->glTexCoord2f(0.0, 1.0); gl->glVertex2f(+size / 2.0, - size * (isMan ? 2.0 : 1.0) + voffset);
				gl->glTexCoord2f(0.0, 0.0); gl->glVertex2f(+size / 2.0, 0.0 + voffset);
				gl->glTexCoord2f(1.0, 0.0); gl->glVertex2f(-size / 2.0, 0.0 + voffset);
			gl->glEnd();
		}
	}

	gl->glPopMatrix();
}

bool One::onFeet()
{
	return (this->y == 0);
}

void One::setRight(bool right)
{
	this->right = right;
}

void One::setLastX(GLfloat lastX)
{
	this->lastX = lastX;
}

void One::setLastY(GLfloat lastY)
{
	this->lastY = lastY;
}

void One::setLastAngle(GLfloat lastAngle)
{
	this->lastAngle = lastAngle;
}

GLfloat One::getLastX()
{
	return this->lastX;
}

GLfloat One::getLastY()
{
	return this->lastY;
}

void One::setImmortal(bool immortal)
{
	this->immortal = immortal;
}

bool One::isImmortal()
{
	return this->immortal;
}
