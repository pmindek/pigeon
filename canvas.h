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


#define C  0
#define CS 1
#define D  2
#define DS 3
#define E  4
#define F  5
#define FS 6
#define G  7
#define GS 8
#define A  9
#define AS 10
#define B  11

#define DULL 12
#define MAN 24

#define GAME_LENGTH 1000
#define GAME_CRUMB_COUNT 1200
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

	inline bool isInRange(qreal pigeonPos, bool right)
	{
		return abs(this->position - pigeonPos - vj->c("crumb offset").toDouble() * (right ? 1.0 : -1.0) ) < vj->c("crumb range").toDouble();
	}

	VJMini *vj;

	qreal position;
	int type;

	int period = 1200;

	qreal minSize = 0.8;
	qreal maxSize = 1.1;

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
	int getCurrentNote();
	void handleMovement(qint64 t);

protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	void keyPressEvent(QKeyEvent *e);
	void keyReleaseEvent(QKeyEvent *e);

	int realityCounter = 0;

	inline bool isMan()
	{
		if (realityCounter > 0)
		{
			return (timeOfDeath <= 0);
		}

		return timeOfDeath > 0;
	}

	inline QString man()
	{
		return (isMan() ? "-m" : "");
	}

	bool healthFreeze = true;

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
	qreal lifeLength;
	quint64 timeOfDeath;
	quint64 timeOfBirb;

	quint64 ptmAmountFade;
	quint64 mtpAmountFade;

	qreal health;
	qreal addedHealth;
	QList<Crumb> crumbs;

	qreal ptmAmount;
	qreal mtpAmount;

	//sounds
	FMOD_SYSTEM      *system;

	FMOD_SOUND       *songCalm;
	FMOD_SOUND       *songPigeon;
	FMOD_SOUND       *songMan;
	FMOD_SOUND       *songBeat;
	FMOD_SOUND       *songArp;

	FMOD_CHANNEL     *channelCalm;
	FMOD_CHANNEL     *channelPigeon;
	FMOD_CHANNEL     *channelMan;
	FMOD_CHANNEL     *channelBeat;
	FMOD_CHANNEL     *channelArp;

	FMOD_CHANNEL     *noteChannel;

	FMOD_SOUND       *sounds[1];
	FMOD_SOUND       *notes[36];
	FMOD_SOUND		 *ptm;
	FMOD_SOUND		 *mtp;

	FMOD_SOUNDGROUP  *soundgroup;


signals:
	void kill();

};

#endif // CANVAS_H
