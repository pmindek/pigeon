#ifndef CANVAS_H
#define CANVAS_H

#include <QOpenGLWidget>
#include <QSharedPointer>
#include <QOpenGLFunctions_2_0>
#include "util/vjmini/vjmini.h"

typedef QSharedPointer<NNShader> PNNShader;

class Canvas : public QOpenGLWidget, public QOpenGLFunctions_2_0
{
	Q_OBJECT
public:
	explicit Canvas(QWidget *parent = nullptr);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	VJMini *vj;

signals:

};

#endif // CANVAS_H
