#include "IEchoXWidgetComponent.h"
#include <QHBoxLayout>

IEchoXWidgetComponent::IEchoXWidgetComponent()
{
	mWindow = QSharedPointer<QWindow3D>::create();
}

void IEchoXWidgetComponent::initialize()
{
}

void IEchoXWidgetComponent::activate()
{
	widget()->resize(desiredSize());
	mWindow->setupBody(widget());
	mWindow->show();
}

void IEchoXWidgetComponent::deactivate()
{
	mWindow->close();
}

QQuadF IEchoXWidgetComponent::getQuad() const
{
	return mWindow->getGlobalQuad();
}

void IEchoXWidgetComponent::setQuad(QQuadF inQuad)
{
	mWindow->setGlobalQuad(inQuad);
}
