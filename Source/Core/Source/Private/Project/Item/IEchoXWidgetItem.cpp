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
	widget()->resize(desiredSize());
	mWindow->setupBody(widget());
	mWindow->show();
}

void IEchoXWidgetItem::deactivate()
{
	mWindow->close();
}

QQuadF IEchoXWidgetItem::getQuad() const
{
	return mWindow->getGlobalQuad();
}

void IEchoXWidgetItem::setQuad(QQuadF inQuad)
{
	mWindow->setGlobalQuad(inQuad);
}

QWindow3D* IEchoXWidgetItem::getWindow() const
{
	return mWindow.get();
}

