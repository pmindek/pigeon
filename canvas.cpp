#include "canvas.h"

Canvas::Canvas(QWidget *parent) : QOpenGLWidget(parent)
{

}

void Canvas::initializeGL()
{
	this->initializeOpenGLFunctions();

	this->vj = new VJMini(this);

	this->vj->loadScript("script.json");
}

void Canvas::resizeGL(int w, int h)
{

}

void Canvas::paintGL()
{
	vj->shader("shader")->bind();

	glBegin(GL_QUADS);
		glVertex2f(-1.0, -1.0);
		glVertex2f( 1.0, -1.0);
		glVertex2f( 1.0, 1.0);
		glVertex2f(-1.0,  1.0);
	glEnd();

	vj->shader("shader")->release();
}
