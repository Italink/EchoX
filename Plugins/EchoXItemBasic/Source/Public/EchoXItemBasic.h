#ifndef EchoXItemBasic_h__
#define EchoXItemBasic_h__

#include "Plugin/IEnginePlugin.h"
#include "QLibrary"

class EchoXItemBasic: public IEnginePlugin {
protected:
	Info info() override;
	void startup() override;
	void shutdown() override;
private:
};

#endif // EchoXItemBasic_h__