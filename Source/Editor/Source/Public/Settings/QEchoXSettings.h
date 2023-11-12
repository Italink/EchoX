#ifndef QEchoXSettings_h__
#define QEchoXSettings_h__

#include "Settings/IEchoXSettings.h"
#include "EchoXEditorAPI.h"

class ECHOXEDITOR_API QEchoXSettings : public IEchoXSettings {
	Q_OBJECT
	Q_CONF_VAR(QString, ProjectsDir) = QString("./Projects");
	Q_CONF_VAR(QString, PluginsDir) = QString("./Plugins");
};


#endif // QEchoXSettings_h__
