#ifndef QAudioAnalyseSettings_h__
#define QAudioAnalyseSettings_h__

#include "Settings/IEchoXSettings.h"

class QAudioAnalyseSettings : public IEchoXSettings {
	Q_OBJECT
public:
	ECHOX_DECLARE_SETTINGS(QAudioAnalyseSettings, AudioAnalyse)
private:
	QWidget* createWidget() override;

};

#endif // QAudioAnalyseSettings_h__
