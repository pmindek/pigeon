#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	MainWindow _;
	_.show();

	return a.exec();
}
