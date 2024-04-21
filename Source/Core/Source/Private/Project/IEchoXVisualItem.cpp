#include "IEchoXVisualItem.h"

bool IEchoXVisualItem::onItemPlacing(QPointF scenePosition)
{
	return true;
}

bool IEchoXVisualItem::isVisible()
{
	return isActivated() && bVisibility;
}

void IEchoXVisualItem::setVisibility(bool bVisible)
{
	bVisibility = bVisible;
}

