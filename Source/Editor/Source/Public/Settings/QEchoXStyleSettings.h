#ifndef QEchoXStyleSettings_h__
#define QEchoXStyleSettings_h__

#include "Settings/IEchoXSettings.h"

class QEchoXStyleSettings : public IEchoXSettings {
	Q_OBJECT
public:
	ECHOX_DECLARE_SETTINGS(QEchoXStyleSettings,Style);
	ECHOX_CONF_VAR(QColor, ThemeColor) = QColor(112, 115, 222);
	ECHOX_CONF_VAR(float, ShadowWidth) = 5;
};

#endif // QEchoXStyleSettings_h__
