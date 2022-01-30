#ifndef MOVEABLE_H
#define MOVEABLE_H

#include <QtOpenGL/QGL>

class Moveable
{
public:
    Moveable();

	void setTexture(GLuint textureId);
	void rotate(GLfloat angle);
	void resize(GLfloat size);
	void move(GLfloat x, GLfloat y);
	void setAlpha(GLfloat alpha);
	GLfloat getX();
	GLfloat getY();

//protected:
	GLfloat x;
	GLfloat y;
	GLfloat alpha;
	GLfloat size;
	GLfloat angle;
	GLuint textureId;
};

#endif // MOVEABLE_H
