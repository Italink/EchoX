#include "IEchoXSettings.h"
#include "DetailView/QDetailView.h"

QWidget* IEchoXSettings::createWidget()
{
	QDetailView* view = new QDetailView;
	view->setObject(this);
	return view;
}

