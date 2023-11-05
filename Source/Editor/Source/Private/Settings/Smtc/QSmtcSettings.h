#ifndef QSmtcSettings_h__
#define QSmtcSettings_h__

#include "Settings/IEchoXSettings.h"

class QSmtcSettings : public IEchoXSettings {
public:
	QString path() override;
	QWidget* createWidget() override;
};

#endif // QSmtcSettings_h__
