#ifndef CustomPlugin_h__
#define CustomPlugin_h__

#include "Plugin/IEnginePlugin.h"

class CustomPlugin: public IEnginePlugin {
protected:
	Info info() override;
	void startup() override;
	void shutdown() override;
};

#endif // CustomPlugin_h__