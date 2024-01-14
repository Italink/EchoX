#ifndef QEchoXPluginsSettings_h__
#define QEchoXPluginsSettings_h__

#include "Settings/IEchoXSettings.h"

class QEchoXPluginsSettings : public IEchoXSettings {
	Q_OBJECT
public:
	ECHOX_DECLARE_SETTINGS(QEchoXPluginsSettings, Plugins);
public:
	QEchoXPluginsSettings();
	virtual QQuickItem* createView(QQmlEngine* engine) override;
};

#endif // QEchoXPluginsSettings_h__
