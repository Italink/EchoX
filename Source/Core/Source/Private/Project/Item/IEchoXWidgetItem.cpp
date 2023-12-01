#include "IEchoXWidgetItem.h"
#include <QHBoxLayout>

IEchoXWidgetItem::IEchoXWidgetItem()
{
	mWindow = QSharedPointer<QWindow3D>::create();
}

void IEchoXWidgetItem::initialize()
{
}

void IEchoXWidgetItem::activate()
{
	mWindow->setupBody(widget());
	mWindow->show();
}

void IEchoXWidgetItem::deactivate()
{
	mWindow->close();
}

