#ifndef QCustomSettings_h__
#define QCustomSettings_h__

#include "Settings/IEchoXSettings.h"
#include "QColor"

class QCustomSettings : public IEchoXSettings {
	Q_OBJECT
public:
	ECHOX_DECLARE_SETTINGS(QCustomSettings, Custom);
	ECHOX_CONF_VAR(QColor, ThemeColor) = QColor(112, 115, 222);
	ECHOX_CONF_VAR(float, ShadowWidth) = 5;
};

#endif // QCustomSettings_h__
