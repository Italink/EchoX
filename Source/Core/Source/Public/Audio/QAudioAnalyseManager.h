#ifndef QAudioAnalyseManager_h__
#define QAudioAnalyseManager_h__

#include <QObject>
#include "EchoXCoreAPI.h"
#include <QAudioFormat>
#include <QSharedPointer>
#include <QTimer>
#include <QThread>

class XAudioSink;
class IAudioCapture;
class QAudioAnalyseInformant;
template<typename _Ty> class Gist;

enum class EAudioWindowType
{
	RectangularWindow,
	HanningWindow,
	HammingWindow,
	BlackmanWindow,
	TukeyWindow
};

class ECHOXCORE_API QAudioAnalyseManager: public QObject
{
	Q_OBJECT
	friend class QAudioAnalyseInformant;
public:
	static QAudioAnalyseManager& Get();
	std::shared_ptr<QAudioAnalyseInformant> createInformant(int frameSize = 1024, EAudioWindowType w = EAudioWindowType::HanningWindow);

	void startup();
	void shutdown();

	void tick();

	int getTickInterval() const;
	QAudioFormat getCurrentFormat();
	std::vector<float> popAudio(const size_t desiredSize);

	void setTickInterval(int val);
private:
	QAudioAnalyseManager() {};
	void registerInformant(QAudioAnalyseInformant* informant);
	void unregisterInformant(QAudioAnalyseInformant* informant);
private:
	QThread mTickThread;
	QTimer mTickTimer;

	std::shared_ptr<XAudioSink> mAudioSink;
	std::shared_ptr<IAudioCapture> mAudioCapture;
	QList<QAudioAnalyseInformant*> mInformants;
};

class ECHOXCORE_API QAudioAnalyseInformant {
	friend class QAudioAnalyseManager;
public:
	QAudioAnalyseInformant();
	~QAudioAnalyseInformant();

	void setAudioFrameSize(int v);
	void setWindowType(EAudioWindowType w);
	int getSampleRate() const;

	std::vector<float> getMagnitudeSpectrum();
	std::vector<float> getMelFrequencySpectrum();
	std::vector<float> getMelFrequencyCepstralCoefficients();
	float getPeakEnergy();
	float getRootMeanSquare();
	float getZeroCrossingRate();
	float getSpectralCentroid();
	float getSpectralCrest();
	float getSpectralFlatness();
	float getSpectralRolloff();
	float getSpectralKurtosis();
	float getEnergyDifference();
	float getSpectralDifference();
	float getSpectralDifferenceHWR();
	float getComplexSpectralDifference();
	float getHighFrequencyContent();
	float getPitch();
private:
	void ensureGist();
	void process();
private:
	int mDesiredAudioFrameSize = 128;
	EAudioWindowType mDesiredWindowType = EAudioWindowType::HanningWindow;
	std::shared_ptr<Gist<float>> mGist;
};

#endif // QAudioAnalyseManager_h__
