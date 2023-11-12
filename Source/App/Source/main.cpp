#include <QApplication>
#include "QEchoXApplication.h"

int main(int argc, char** argv)
{
	QEchoXApplication::preInitialize();
	QEchoXApplication app(argc, argv);
	//view.show();   
	return app.exec();  
}        