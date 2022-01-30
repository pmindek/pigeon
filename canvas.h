#ifndef CANVAS_H
#define CANVAS_H

#include <QOpenGLWidget>
#include <QSharedPointer>
#include <QOpenGLFunctions_2_0>
#include "util/vjmini/vjmini.h"
#include <QKeyEvent>
#include <QMatrix4x4>

#include "one.h"

#include "libs/fmod/inc/fmod.h"
#include "libs/fmod/inc/fmod_errors.h"

//#define SOUNDS_COUNT 1

typedef QSharedPointer<NNShader> PNNShader;

class Canvas : public QOpenGLWidget, public QOpenGLFunctions_2_0
{
	Q_OBJECT
public:
	explicit Canvas(QWidget *parent = nullptr);
	~Canvas();

private:
	void handleMoving();

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

	int w, h;
	bool horizontalStripes;

	QTime time;
	qreal offset = 0.0;

	QMatrix4x4 projection;
	QMatrix4x4 modelview;

	One *pigeon;
	int peck = 0;
	int runLeft = 0;
	int runRight = 0;
	qreal runLeftSlowDown;
	qreal runRightSlowDown;

	bool firstTime, animationMode, tutorialMode;
	int tutorialState;
	int difficulty;

	QList<NNShader *> post;

	qreal runSpeed = 250.0;
	qreal run = 0.0;

	VJMini *vj;

	//sounds
	FMOD_SYSTEM      *system;
	FMOD_SOUND       *sound;
	FMOD_CHANNEL     *channel;

	FMOD_SOUND       *sounds[1];

	FMOD_SOUNDGROUP  *soundgroup;


signals:
	void kill();

};

#endif // CANVAS_H
