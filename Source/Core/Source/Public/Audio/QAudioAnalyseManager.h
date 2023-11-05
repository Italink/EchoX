#ifndef QAudioAnalyseManager_h__
#define QAudioAnalyseManager_h__

#include <QObject>
#include <QAudioFormat>
#include <QSharedPointer>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QMap>
#include "EchoXCoreAPI.h"
#include "ConstantQTransform.h"

class XAudioSink;
class IAudioCapture;
class QAudioAnalyseInformant;
template<typename _Ty> class Gist;

enum class ECHOXCORE_API EAudioWindowType
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
	QMutex* getMutex();
	QAudioFormat getCurrentFormat();
	std::vector<float> popAudio(const size_t desiredSize);

	void setTickInterval(int val);
private:
	QAudioAnalyseManager() {};
	void registerInformant(QAudioAnalyseInformant* informant);
	void unregisterInformant(QAudioAnalyseInformant* informant);
private:
	QMutex mMutex;
	QThread mTickThread;
	QTimer* mTickTimer = nullptr;
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

	QVector<float> getMagnitudeSpectrum();
	QVector<float> getMelFrequencySpectrum();
	QVector<float> getMelFrequencyCepstralCoefficients();
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

	void setCqtEnabled(bool bEnable);
	void setCqtSetting(int hopSize = 256, int binsPerOctave = 12, int octaveNumber = 7);
	std::vector<Cqt::ScheduleElement> getCqtSchedule();
	QVector<float> getCqtMagnitudeSpectrum(const Cqt::ScheduleElement schedule);
private:
	void ensureGist();
	void ensureCqt();
	void process();
private:
	int mDesiredAudioFrameSize = 128;
	EAudioWindowType mDesiredWindowType = EAudioWindowType::HanningWindow;
	std::shared_ptr<Gist<float>> mGist;

	struct CqtSetting{
		bool bEnabled = true;
		int hopSize = 256;
		int binsPerOctave = 12;
		int octaveNumber = 7;
	};
	CqtSetting mDesiredCqtSetting;
	std::shared_ptr<Cqt::ConstantQTransform> mCqt;

	struct SpectrumCache {
		QVector<float> MagnitudeSpectrum;
		QVector<float> MelFrequencySpectrum;
		QVector<float> MelFrequencyCepstralCoefficients;
		QMap<int,QVector<float>> CqtMagnitudeSpectrum;
	}mSpectrumCache;

	float mSmoothingUp = 0.0f;
	float mSmoothingDown = 0.5f;
};

#endif // QAudioAnalyseManager_h__
