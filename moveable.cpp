#include "moveable.h"

Moveable::Moveable()
{
}

void Moveable::setTexture(GLuint textureId)
{
	this->textureId = textureId;
}

void Moveable::rotate(GLfloat angle)
{
	this->angle = angle;
}

void Moveable::resize(GLfloat size)
{
	this->size = size;
}

void Moveable::move(GLfloat x, GLfloat y)
{
	if (x < 0)
		x = 0;

	this->x = x;
	this->y = y;
}

void Moveable::setAlpha(GLfloat alpha)
{
	this->alpha = alpha;
}

GLfloat Moveable::getX()
{
	return this->x;
}

GLfloat Moveable::getY()
{
	return this->y;
}
