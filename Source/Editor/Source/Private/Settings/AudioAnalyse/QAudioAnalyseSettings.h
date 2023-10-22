#ifndef QAudioAnalyseSettings_h__
#define QAudioAnalyseSettings_h__

#include "Settings/IEchoXSettings.h"

class QAudioAnalyseSettings : public IEchoXSettings {
public:
	QString path() override;
	QWidget* createWidget() override;
};

#endif // QAudioAnalyseSettings_h__
