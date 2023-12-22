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

