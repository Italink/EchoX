#include <QApplication>
#include <QPainter>
#include <QWindow>
#include "QSmtcManager.h"

int main(int argc, char** argv) {
	QApplication app(argc, argv);
	QSmtcManager mgr;
	
	return app.exec();
}