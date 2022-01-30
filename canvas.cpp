#include "canvas.h"

Canvas::Canvas(QWidget *parent) : QOpenGLWidget(parent)
{
	this->setFocusPolicy(Qt::StrongFocus);
	this->grabKeyboard();
}

Canvas::~Canvas()
{
	FMOD_Sound_Release(songCalm);
	FMOD_Sound_Release(songBeat);

	FMOD_System_Close(system);
	FMOD_System_Release(system);

	delete this->vj;
}

void Canvas::initializeGL()
{
	this->initializeOpenGLFunctions();

	this->vj = new VJMini(this);
	this->vj->nn->resize(this->width(), this->height());

	this->vj->nn->addFbo("main");
	this->vj->nn->addFbo(0);
	this->vj->nn->addFbo(1);

	connect(this->vj, SIGNAL(updated()), this, SLOT(update()));

	this->vj->loadScript("script.json");

	this->post << this->vj->shader("vision")
			   << this->vj->shader("chroma")
				  ;



	glDisable(GL_DEPTH_TEST);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	glEnable(GL_BLEND);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

	this->pigeon = new One(this);

	this->modelview.setToIdentity();

	this->projection.setToIdentity();
	this->projection.ortho(-0.89, 16.89, 10.0, 0.0, 0.0, 1.0);

	//gameplay

	for (int i = 0; i < GAME_CRUMB_COUNT; i++)
	{
		this->crumbs << Crumb(NNU::random() * (qreal) GAME_LENGTH, this->vj);
	}





	//sound
	channelCalm = 0;
	channelBeat = 0;
	channelArp = 0;
	noteChannel = 0;

	FMOD_System_Create(&system);
	FMOD_System_Init(system, 5, FMOD_INIT_NORMAL, NULL);


	//notes

	QStringList noteNames;
	noteNames << "c" << "cs" << "d" << "ds" << "e" << "f" << "fs" << "g" << "gs" << "a" << "as" << "b";

	QStringList noteModifiers;
	noteModifiers << "" << "-";

	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 12; i++)
		{
			int noteId = 12 * j + i;

			FMOD_System_CreateSound(system, ("music/" + noteModifiers[j] + noteNames[i] + ".wav").toLatin1(), FMOD_SOFTWARE, 0, &notes[noteId]);
			FMOD_Sound_SetMode(notes[noteId], FMOD_LOOP_OFF);
		}
	}



	/*FMOD_System_CreateSound(system, "hit.wav", FMOD_SOFTWARE, 0, &hitSound);
	FMOD_Sound_SetMode(hitSound, FMOD_LOOP_OFF);

	FMOD_System_CreateSound(system, "hoop.wav", FMOD_SOFTWARE, 0, &hoopSound);
	FMOD_Sound_SetMode(hoopSound, FMOD_LOOP_OFF);

	FMOD_System_CreateSound(system, "newmail.wav", FMOD_SOFTWARE, 0, &creepSound);
	FMOD_Sound_SetMode(creepSound, FMOD_LOOP_OFF);

	FMOD_System_CreateSound(system, "again.wav", FMOD_SOFTWARE, 0, &againSound);
	FMOD_Sound_SetMode(againSound, FMOD_LOOP_OFF);*/

	FMOD_System_CreateStream(system, "music/pigeon-calm.wav", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &songCalm);
	FMOD_System_CreateStream(system, "music/pigeon-beat.wav", FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &songBeat);
	FMOD_System_CreateStream(system, "music/pigeon-arp.wav",  FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &songArp);


	FMOD_System_CreateSoundGroup(system, "All", &soundgroup);
	FMOD_Sound_SetSoundGroup(songCalm, soundgroup);
	FMOD_Sound_SetSoundGroup(songBeat, soundgroup);
	FMOD_Sound_SetSoundGroup(songArp, soundgroup);

	for (int i = 0; i < 12; i++)
	{
		FMOD_Sound_SetSoundGroup(notes[i], soundgroup);
	}

	/*FMOD_Sound_SetSoundGroup(highSound, soundgroup);
	FMOD_Sound_SetSoundGroup(hitSound, soundgroup);
	FMOD_Sound_SetSoundGroup(hoopSound, soundgroup);
	FMOD_Sound_SetSoundGroup(creepSound, soundgroup);
	FMOD_Sound_SetSoundGroup(againSound, soundgroup);*/

	FMOD_Channel_SetVolume(channelBeat, 0.0);
	FMOD_System_Update(system);
	FMOD_Channel_SetVolume(channelArp, 0.0);
	FMOD_System_Update(system);

	FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, songCalm, 0, &channelCalm);
	FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, songBeat, 0, &channelBeat);
	FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, songArp, 0, &channelArp);


	//FMOD_SoundGroup_SetVolume(soundgroup, 0.0);
	FMOD_System_Update(system);

	time.start();

	this->offset = 0.0;
	health = 0.0;
	addedHealth = 0.0;
}

void Canvas::resizeGL(int width, int height)
{
	qreal ar = /*0.625*/ 0.5625;

	qreal rr = (GLfloat) this->height() / (GLfloat) this->width();

	if (rr == ar)
	{
		//glViewport(0, 0, this->width(), this->height());
		w = this->width();
		h = this->height();
	}
	else if (rr < ar)
	{
		h = this->height();
		w = (int) ((GLfloat) this->height() / ar);
		//glViewport((this->width() - w) / 2.0, 0, w, h);
		horizontalStripes = false;
	}
	else
	{
		w = this->width();
		h = (int) ((GLfloat) this->width() * ar);
		//glViewport(0, (this->height() - h) / 2.0, w, h);
		horizontalStripes = true;
	}

	glViewport(0, 0, w, h);

	this->vj->nn->resize(w, h);

	this->update();
}

void Canvas::paintGL()
{
	qint64 t = time.elapsed();

	handleMovement(t);


	FMOD_Channel_SetVolume(channelArp, pow(1.0 - this->health, 3.0));

	if (health > 0.5)
	{
		FMOD_Channel_SetVolume(channelBeat, 0.0);
	}
	else if (health > 0.4)
	{
		FMOD_Channel_SetVolume(channelBeat, 1.0 - (health - 0.4) / 0.1);
	}
	else
	{
		FMOD_Channel_SetVolume(channelBeat, 1.0);
	}

	FMOD_System_Update(system);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.89, 16.89, 10.0, 0.0, 0.0, 1.0);
	//glLoadMatrixf(this->projection.data());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0, 0, w, h);

	vj->nn->fbo("main")->bind();


	//vj->shader("shader")->bind();

	glUseProgram(0);

	glClearColor(0.0, 0.0, 0.0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	//background
	glDisable(GL_TEXTURE_2D);
	glColor4f(0.8, 0.82, 0.86, 1);
	glBegin(GL_QUADS);
		glVertex2f(-0.89, 0.0);
		glVertex2f(16.89, 0.0);
		glVertex2f(16.89, 10.0);
		glVertex2f(-0.89, 10.0);
	glEnd();


	//back grass
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0, 1.0, 1.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, vj->texture("bg sidewalk"));
	glBegin(GL_QUADS);
		glTexCoord2f(1.3 - (offset / 17.78) * 2.5, 0.99); glVertex2f(-0.89, 10.0 - 1.2);
		glTexCoord2f(3.8 - (offset / 17.78) * 2.5, 0.99); glVertex2f(16.89, 10.0 - 1.2);
		glTexCoord2f(3.8 - (offset / 17.78) * 2.5, 0.0); glVertex2f(16.89, 10.0);
		glTexCoord2f(1.3 - (offset / 17.78) * 2.5, 0.0); glVertex2f(-0.89, 10.0);
	glEnd();


	glEnable(GL_TEXTURE_2D);


	//breadcrumbs
	for (int i = 0; i < this->crumbs.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, this->vj->texture("crumb" + QString::number(this->crumbs[i].type)));

		qreal x = this->crumbs[i].position + offset;
		qreal y = 9.0;
		qreal s = this->crumbs[i].getSize(t) / 2.0;

		if (this->crumbs[i].isInRange(pigeon->getX(), pigeon->isRight()))
		{
			glColor4f(1.0, 0.9, 0.9, 1.0);
		} else
		{
			glColor4f(1.0, 1.0, 1.0, 1.0);
		}

		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 1.0); glVertex2f(x - s, y - s);
			glTexCoord2f(1.0, 1.0); glVertex2f(x + s, y - s);
			glTexCoord2f(1.0, 0.0); glVertex2f(x + s, y + s);
			glTexCoord2f(0.0, 0.0); glVertex2f(x - s, y + s);
		glEnd();
	}



	//draw pigeon
	GLuint pigeonTextures[2] = {vj->texture("pigeon0"), vj->texture("pigeon1")};

	GLuint pigeonTexture = pigeonTextures[0];

	if (runLeft > 0 || runRight > 0)
	{
		pigeonTexture = pigeonTextures[(t % 300) / 150 ];
	}

	glBindTexture(GL_TEXTURE_2D, pigeonTexture);

	modelview.setToIdentity();
	modelview.translate(pigeon->x + offset, 10 - pigeon->y, 0.0);
	modelview.rotate(pigeon->angle, 0.0, 0.0, 1.0);

	vj->shader("peck")->bind();
	vj->shader("peck")->getShader()->setUniformValue("modelview", modelview);
	vj->shader("peck")->getShader()->setUniformValue("projection", projection);
	vj->shader("peck")->getShader()->setUniformValue("time", (GLfloat) pigeon->getPeck());

	/*glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex2f( 6.0, 9.0 + 0.45);
		glTexCoord2f(1.0, 0.0); glVertex2f(10.0, 9.0 + 0.45);
		glTexCoord2f(1.0, 1.0); glVertex2f(10.0, 5.0 + 0.45);
		glTexCoord2f(0.0, 1.0); glVertex2f( 6.0, 5.0 + 0.45);
	glEnd();*/

	pigeon->display(offset, pigeonTexture);

	vj->shader("peck")->release();

	vj->nn->fbo("main")->release();




	glOrtho(-1, 1, -1, 1, -1, 1);

	/*glColor4f(1,1,1,1);
	glBindTexture(GL_TEXTURE_2D, vj->nn->fbo("main")->texture());

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex2f(-0.89, 0.0);
		glTexCoord2f(1.0, 1.0); glVertex2f(16.89, 0.0);
		glTexCoord2f(1.0, 0.0); glVertex2f(16.89, 10.0);
		glTexCoord2f(0.0, 0.0); glVertex2f(-0.89, 10.0);
	glEnd();*/


	//glViewport(0, 0, this->width(), this->height());

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	this->modelview.setToIdentity();



	int counter = 0;

	for (NNShader *shader : this->post)
	{
		vj->nn->fbo(counter % 2)->bind();

		shader->bind();
		glBindTexture(GL_TEXTURE_2D, counter == 0 ? vj->nn->fbo("main")->texture() : vj->nn->fbo(1 - counter % 2)->texture());

		shader->getShader()->setUniformValue("tex0", 0);
		shader->getShader()->setUniformValue("size", QVector2D(this->w, this->h));

		switch (counter)
		{
		case 0:
			shader->getShader()->setUniformValue("run", (GLfloat) this->run);
			break;
		case 1:
			shader->getShader()->setUniformValue("time", (uint) t);
			shader->getShader()->setUniformValue("amount", 12.0f);
			break;
		}

		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 1.0); glVertex2f(-1,1);
			glTexCoord2f(1.0, 1.0); glVertex2f(1,1);
			glTexCoord2f(1.0, 0.0); glVertex2f(1,-1);
			glTexCoord2f(0.0, 0.0); glVertex2f(-1,-1);
		glEnd();
		shader->release();

		vj->nn->fbo(counter % 2)->release();

		counter++;
	}

	if (!horizontalStripes)
	{
		glViewport((this->width() - w) / 2.0, 0, w, h);
	}
	else
	{
		glViewport(0, (this->height() - h) / 2.0, w, h);
	}

	glBindTexture(GL_TEXTURE_2D, counter == 0 ? vj->nn->fbo("main")->texture() : vj->nn->fbo(1 - counter % 2)->texture());
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex2f(-1,1);
		glTexCoord2f(1.0, 1.0); glVertex2f(1,1);
		glTexCoord2f(1.0, 0.0); glVertex2f(1,-1);
		glTexCoord2f(0.0, 0.0); glVertex2f(-1,-1);
	glEnd();


	//healthbar
	qreal healthbarSize = 10;

	glBindTexture(GL_TEXTURE_2D, this->vj->texture("healthbar-shadow"));
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex2f(8.0 - healthbarSize / 2.0, 1.0);
		glTexCoord2f(1.0, 1.0); glVertex2f(8.0 + healthbarSize / 2.0, 1.0);
		glTexCoord2f(1.0, 0.0); glVertex2f(8.0 + healthbarSize / 2.0, 1.0 + healthbarSize / 8.0);
		glTexCoord2f(0.0, 0.0); glVertex2f(8.0 - healthbarSize / 2.0, 1.0 + healthbarSize / 8.0);
	glEnd();

	vj->shader("healthbar")->bind();
	modelview.setToIdentity();
	vj->shader("healthbar")->getShader()->setUniformValue("modelview", modelview);
	vj->shader("healthbar")->getShader()->setUniformValue("projection", projection);
	vj->shader("healthbar")->getShader()->setUniformValue("health", (GLfloat) health);

	glBindTexture(GL_TEXTURE_2D, this->vj->texture("healthbar"));
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0); glVertex2f(8.0 - healthbarSize / 2.0, 1.0);
		glTexCoord2f(1.0, 1.0); glVertex2f(8.0 + healthbarSize / 2.0, 1.0);
		glTexCoord2f(1.0, 0.0); glVertex2f(8.0 + healthbarSize / 2.0, 1.0 + healthbarSize / 8.0);
		glTexCoord2f(0.0, 0.0); glVertex2f(8.0 - healthbarSize / 2.0, 1.0 + healthbarSize / 8.0);
	glEnd();
	vj->shader("healthbar")->release();



	//vj->shader("shader")->release();

	offset = -pigeon->getX() + 8.0;

	getCurrentNote();
}

int Canvas::getCurrentNote()
{
	uint pos;
	FMOD_Channel_GetPosition(channelCalm, &pos, FMOD_TIMEUNIT_MS);

	int chord = (pos / 2000) % 8;

	int chords[8 * 3] = {
		E, G, B,
		D, FS,A,
		C, E, G,
		D, FS,A,
		D, G, B,
		D, FS,A,
		E, A, C,
		E, G, B
	};

	return chords[chord * 3 + rand() % 3];

}

void Canvas::handleMovement(qint64 t)
{
	qreal tmp = 0;

	qreal tmpX = pigeon->getX();

	qreal runTime = 0.0;

	if (runLeft > 0)
	{
		runTime = (qreal) (t - runLeft);

		tmp = runTime / runSpeed;

		pigeon->move(pigeon->getLastX() - tmp, pigeon->getY());
	}
	if (runRight > 0)
	{
		runTime = (qreal) (t - runRight);

		tmp = runTime / runSpeed;

		pigeon->move(pigeon->getLastX() + tmp, pigeon->getY());
	}
	if (peck > 0)
	{
		tmp = (GLfloat) ((t - peck) / 200.0);

		QEasingCurve e(QEasingCurve::Linear);
		tmp = e.valueForProgress(tmp);

		if (tmp >= 1.0)
		{
			peck = 0;
			tmp = 0.0;
		}
		pigeon->setPeck(tmp);

		if (peck > 0.5 && !ate)
		{
			ate = true;
			bool success = false;

			for (int i = 0; i < this->crumbs.size(); i++)
			{
				if (this->crumbs[i].isInRange(pigeon->getX(), pigeon->isRight()))
				{
					this->crumbs.takeAt(i);

					qreal inc = vj->c("health increment").toDouble();

					this->addedHealth += qMin(inc, 1.0 - this->health);
					success = true;

					break;
				}
			}

			FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, notes[getCurrentNote() + (success ? 0 : DULL)], 0, &noteChannel);
			FMOD_System_Update(system);
		}
	}

	if (runTime < 1000 && runTime >= 0)
	{
		run = runTime / 1000.0;
	}
	else
	{
		if (runLeft > 0 || runRight > 0)
		{
			run = 1.0;
		}
		else
		{
			run = 0.0;
		}
	}


	health = qMin(1.0, qMax(0.0, 1.0 - (qreal) t / 10000.0 + addedHealth));
}



void Canvas::keyPressEvent(QKeyEvent *e)
{
	if (time.elapsed() < 100)
	{
		return;
	}

	if (e->isAutoRepeat())
	{
		return;
	}

	if (tutorialMode && tutorialState < 2)
	{
		switch (e->key())
		{
		case Qt::Key_Escape:
			emit kill();
		break;

		case Qt::Key_Left:
		case Qt::Key_Right:
		case Qt::Key_Up:
		case Qt::Key_Down:
			tutorialState = 1 - tutorialState;
		break;

		case Qt::Key_Space:
		case Qt::Key_Return:
			difficulty = tutorialState;
			tutorialState = 2;
			//goAgain();
			return;
		break;
		}
	}

	if (tutorialMode && tutorialState >= 2)
	{
		tutorialMode = false;
		animationMode = true;
		time.start();
	}


	if (e->modifiers().testFlag(Qt::ControlModifier))
	{
		runSpeed = 20.0;
	}
	else
	{
		runSpeed = 250.0;
	}

	switch (e->key())
	{
	case Qt::Key_Escape:
		emit kill();
	break;

	case Qt::Key_Left:
		if (runLeft != 0 || animationMode) break;
		pigeon->setLastX(pigeon->getX());
		pigeon->setRight(false);
		runLeft = time.elapsed();
		runRight = 0;
		run = 0.0;
	break;

	case Qt::Key_Right:
		if (runRight != 0 || animationMode) break;
		pigeon->setLastX(pigeon->getX());
		pigeon->setRight(true);
		runRight = time.elapsed();
		runLeft = 0;
		run = 0.0;
	break;

	case Qt::Key_Up:
	break;
	case Qt::Key_Down:
	break;

	case Qt::Key_Space:
		if (peck > 0 || animationMode) break;
		peck = time.elapsed();
		ate = false;
	break;

	default:
	break;
	}
}

void Canvas::keyReleaseEvent(QKeyEvent *e)
{
	if (e->isAutoRepeat())
	{
		return;
	}

	switch (e->key())
	{
	case Qt::Key_Left:
		runLeftSlowDown = time.elapsed();
		runLeft = 0;
		run = 0.0;
	break;

	case Qt::Key_Right:
		runRightSlowDown = time.elapsed();
		runRight = 0;
		run = 0.0;
	break;

	case Qt::Key_Down:
	break;


	case Qt::Key_P:
	break;

	default:
	break;
	}
}
