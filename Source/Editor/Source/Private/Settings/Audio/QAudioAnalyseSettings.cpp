#include "QAudioAnalyseSettings.h"
#include "QAudioAnalyseView.h"

QWidget* QAudioAnalyseSettings::createWidget()
{
	return new QAudioAnalyseView();
}
