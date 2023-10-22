#ifndef QAudioAnalyseView_h__
#define QAudioAnalyseView_h__

#include <QWidget>
#include <QTimer>
#include "EchoXEditorAPI.h"
#include "Audio/QAudioAnalyseManager.h"

class QAudioOscillogram;
class QAudioSpectrogram;
class QHighFrequencyContentOscillogram;

class QAudioAnalyseView: public QWidget {
public:
	QAudioAnalyseView();
private:
	void updateView();
private:
	QTimer mTimer;
	std::shared_ptr<QAudioAnalyseInformant> mDebugInformant;

	QAudioOscillogram* mRootMeanSquare;
	QAudioOscillogram* mPeakEnergy;
	QAudioOscillogram* mEnergyDifference;
	QAudioOscillogram* mSpectralDifference;

	QAudioSpectrogram* mMagnitudeSpectrum;
	QAudioSpectrogram* mMelFrequencySpectrum;
	QHighFrequencyContentOscillogram* mHighFrequencyContent;
};


#endif // QAudioAnalyseView_h__
