#include <QApplication>
#include "QEchoXApplication.h"
#include "QEchoXMainEditor.h"
#include <QTextEdit>
#include "Window3D/QWindow3D.h"
#include "DetailView/QDetailView.h"


int main(int argc, char** argv)
{
	QEchoXApplication::preInitialize();
	QEchoXApplication app(argc, argv);
	//view.show();   
	return app.exec();  
}        