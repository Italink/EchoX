#ifndef QEchoXStyleSettings_h__
#define QEchoXStyleSettings_h__

#include "Settings/IEchoXSettings.h"

class QEchoXStyleSettings : public IEchoXSettings {
	Q_OBJECT
public:
	ECHOX_DECLARE_SETTINGS(QEchoXStyleSettings);
	ECHOX_CONF_VAR(QColor, ThemeColor) = QColor(112, 115, 222);
};

#endif // QEchoXStyleSettings_h__
