#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	this->resize(1280, 720);

	this->canvas = new Canvas(this);
	this->setCentralWidget(this->canvas);

	QTimer *timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, [=]{
		update();
	});

	timer->start(16);
}
