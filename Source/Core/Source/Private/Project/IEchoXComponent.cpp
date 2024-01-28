#include "IEchoXComponent.h"

bool IEchoXComponent::isActivated() const
{
	return bActivated;
}

void IEchoXComponent::activateInternal()
{
	bActivated = true;
	activate();
}

void IEchoXComponent::deactivateInternal()
{
	deactivate();
	bActivated = false;
}

void IEchoXComponent::setActivated(bool inActivated)
{
	if (inActivated && !bActivated) {
		activate();
	}
	else if (!inActivated&& bActivated) {
		deactivate();
	}
}

bool IEchoXComponent::getActivated() const
{
	return bActivated;
}

