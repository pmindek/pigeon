#include "canvas.h"

Canvas::Canvas(QWidget *parent) : QOpenGLWidget(parent)
{

}

void Canvas::initializeGL()
{
	this->initializeOpenGLFunctions();

	this->vj = new VJMini(this);

	connect(this->vj, SIGNAL(updated()), this, SLOT(update()));

	this->vj->loadScript("script.json");
}

void Canvas::resizeGL(int w, int h)
{

}

void Canvas::paintGL()
{
	//vj->shader("shader")->bind();

	vj->shader("shader")->release();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, vj->texture("noise"));

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
		glTexCoord2f(1.0, 0.0); glVertex2f( 1.0, -1.0);
		glTexCoord2f(1.0, 1.0); glVertex2f( 1.0, 1.0);
		glTexCoord2f(0.0, 1.0); glVertex2f(-1.0,  1.0);
	glEnd();

	//vj->shader("shader")->release();
}
