#ifndef CANVAS_H
#define CANVAS_H

#include <QOpenGLWidget>
#include <QSharedPointer>
#include <QOpenGLFunctions_2_0>
#include "util/vjmini/vjmini.h"
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QElapsedTimer>

#include "one.h"

#include "libs/fmod/inc/fmod.h"
#include "libs/fmod/inc/fmod_errors.h"


#define GAME_LENGTH 300
#define GAME_CRUMB_COUNT 300
#define GAME_CRUMB_TYPES 3

//#define SOUNDS_COUNT 1

typedef QSharedPointer<NNShader> PNNShader;

class Crumb
{
public:
	inline Crumb(qreal position, VJMini *vj)
	{
		this->vj = vj;
		this->position = position;
		this->type = rand() % GAME_CRUMB_TYPES;
		this->phase = NNU::random() * 2.0 * NN_PI;
	}

	inline bool isInRange(qreal pigeonPos)
	{
		return abs(this->position - pigeonPos - vj->c("crumb offset").toDouble() ) < vj->c("crumb range").toDouble();
	}

	VJMini *vj;

	qreal position;
	int type;

	int period = 1200;

	qreal minSize = 0.7;
	qreal maxSize = 1.0;

	qreal phase;

	inline qreal getSize(int t)
	{
		qreal zo = (qreal) (t % period) / (qreal) period;

		return NNU::mix(minSize, maxSize, (1.0 + sin(phase + zo * 2.0 * NN_PI)) * 0.5);
	}
};

class Canvas : public QOpenGLWidget, public QOpenGLFunctions_2_0
{
	Q_OBJECT
public:
	explicit Canvas(QWidget *parent = nullptr);
	~Canvas();

private:
	void handleMovement();

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

	int w, h;
	bool horizontalStripes;

	QElapsedTimer time;
	qreal offset = 0.0;

	QMatrix4x4 projection;
	QMatrix4x4 modelview;

	One *pigeon;
	int peck = 0;
	bool ate = true;
	int runLeft = 0;
	int runRight = 0;
	qreal runLeftSlowDown;
	qreal runRightSlowDown;

	bool firstTime = true, animationMode = 0, tutorialMode = false;
	int tutorialState = 0;
	int difficulty = 0;

	QList<NNShader *> post;

	qreal runSpeed = 250.0;
	qreal run = 0.0;

	VJMini *vj;

	//gameplay
	QList<Crumb> crumbs;

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
