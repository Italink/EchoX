#ifndef QSmtcSettings_h__
#define QSmtcSettings_h__

#include "Settings/IEchoXSettings.h"

class QSmtcSettings : public IEchoXSettings {
	Q_OBJECT
public:
	ECHOX_DECLARE_SETTINGS(QSmtcSettings,Smtc)
	//QWidget* createWidget() override;
};

#endif // QSmtcSettings_h__
