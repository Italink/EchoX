#ifndef QWidgetVFXSettings_h__
#define QWidgetVFXSettings_h__

#include "Settings/IEchoXSettings.h"
#include "IWidgetVFX.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QWidgetVFXSettings : public IEchoXSettings {
	Q_OBJECT
public:
	ECHOX_DECLARE_SETTINGS(QWidgetVFXSettings, WidgetVFX)
	ECHOX_CONF_VAR(QColor, ThemeColor) = QColor(112, 115, 222);
	ECHOX_CONF_VAR(float, ShadowWidth) = 5;
	ECHOX_CONF_VAR(QSharedPointer<IWidgetCloseVFX>, CloseWidgetVFX);

	Q_CLASSINFO("CloseWidgetVFX","Instanced")
};

#endif // QWidgetVFXSettings_h__
