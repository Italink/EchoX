#include "IEchoXItem.h"

void IEchoXItem::activateInternal()
{
	bActivated = true;
	activate();
}

void IEchoXItem::deactivateInternal()
{
	deactivate();
	bActivated = false;
}

bool IEchoXItem::isActivated() const
{
	return bActivated;
}

void IEchoXItem::setActivated(bool inActivated)
{
	if (inActivated && !bActivated) {
		activate();
	}
	else if (!inActivated&& bActivated) {
		deactivate();
	}
}

