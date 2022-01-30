#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	this->resize(1280, 720);

	this->canvas = new Canvas(this);
	this->setCentralWidget(this->canvas);

	connect(canvas, SIGNAL(kill()), this, SLOT(close()));

	QTimer *timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, [=]{
		this->canvas->update();
	});

	timer->start(16);
}
