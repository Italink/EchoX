#include "QAudioAnalyseManager.h"
#include "AudioCapture.h"
#include "AudioSink.h"
#include "Gist.h"
#include <QApplication>

QAudioAnalyseManager& QAudioAnalyseManager::Get()
{
	static QAudioAnalyseManager Instance;
	return Instance;
}

void QAudioAnalyseManager::startup()
{
	mAudioSink = std::make_shared<XAudioSink>();
	mAudioCapture = IAudioCapture::Create();
	mAudioCapture->setSink(mAudioSink);
	mTickTimer = new QTimer;
	mTickTimer->setInterval(30);
	mTickTimer->moveToThread(&mTickThread);
	connect(&mTickThread, SIGNAL(started()), mTickTimer, SLOT(start()));
	connect(&mTickThread, &QThread::finished, mTickTimer, &QObject::deleteLater);
	connect(mTickTimer, &QTimer::timeout, this, &QAudioAnalyseManager::tick, Qt::DirectConnection);
	mAudioCapture->start();
	mTickThread.start();
}

void QAudioAnalyseManager::shutdown()
{
	//Q_ASSERT(mInformants.isEmpty());
	mAudioCapture->stop();
	mTickThread.quit();
	mTickThread.wait();
	mAudioCapture.reset();
	mAudioSink.reset();
}

void QAudioAnalyseManager::tick()
{
	QMutexLocker<QMutex> locker(&mMutex);
	for (auto& informant : mInformants) {
		informant->process();
	}
}

std::shared_ptr<QAudioAnalyseInformant> QAudioAnalyseManager::createInformant(int frameSize /*= 1024*/, EAudioWindowType w)
{
	std::shared_ptr<QAudioAnalyseInformant> informant = std::make_shared<QAudioAnalyseInformant>();
	informant->setAudioFrameSize(frameSize);
	informant->setWindowType(w);
	informant->ensureGist();
	informant->ensureCqt();
	return informant;
}

QMutex* QAudioAnalyseManager::getMutex() 
{
	return &mMutex;
}

QAudioFormat QAudioAnalyseManager::getCurrentFormat()
{
	QAudioFormat format;
	format.setChannelCount(mAudioSink->format().nChannels);
	//format.setSampleFormat(mAudioSink->format().wBitsPerSample);
	format.setSampleRate(mAudioSink->format().nSamplesPerSec);
	return format;
}

std::vector<float> QAudioAnalyseManager::popAudio(const size_t desiredSize)
{
	return mAudioSink->popAudio(desiredSize);
}

void QAudioAnalyseManager::registerInformant(QAudioAnalyseInformant* informant)
{
	mInformants << informant;
}

void QAudioAnalyseManager::unregisterInformant(QAudioAnalyseInformant* informant)
{
	mInformants.removeOne(informant);
}

void QAudioAnalyseInformant::setAudioFrameSize(int v)
{
	mDesiredAudioFrameSize = v;
}

void QAudioAnalyseInformant::setWindowType(EAudioWindowType w)
{
	mDesiredWindowType = w;
}

int QAudioAnalyseInformant::getSampleRate() const
{
	return mGist->getSamplingFrequency();
}

void QAudioAnalyseInformant::setCqtEnabled(bool bEnable)
{
	mDesiredCqtSetting.bEnabled = bEnable;
}

void QAudioAnalyseInformant::setCqtSetting(int hopSize /*= 256*/, int binsPerOctave /*= 12*/, int octaveNumber /*= 7*/)
{
	mDesiredCqtSetting.hopSize = hopSize;
	mDesiredCqtSetting.binsPerOctave = binsPerOctave;
	mDesiredCqtSetting.octaveNumber = octaveNumber;
}

std::vector<Cqt::ScheduleElement> QAudioAnalyseInformant::getCqtSchedule()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mCqt->getCqtSchedule();
}

QVector<float> QAudioAnalyseInformant::getCqtMagnitudeSpectrum(const Cqt::ScheduleElement schedule)
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	mCqt->cqt(schedule);
	auto cqtDomainBuffer = mCqt->getOctaveCqtBuffer(schedule.octave); // the data could now be manipulated in cqt domain
	QVector<float> curr(cqtDomainBuffer.size());
	QVector<float>& last = mSpectrumCache.CqtMagnitudeSpectrum[schedule.octave];
	last.resize(curr.size());
	for (int i = 0; i < curr.size(); i++) {
		const float realD = cqtDomainBuffer[i].real();
		const float imagD = cqtDomainBuffer[i].imag();
		const float magnitude = std::sqrt(std::pow(realD, 2) + std::pow(imagD, 2)) * 100;
		const float lastMagnitude = last[i];
		if (magnitude > lastMagnitude)
			curr[i] = (1. - mSmoothingUp) * magnitude + mSmoothingUp * lastMagnitude;
		else
			curr[i] = (1. - mSmoothingDown) * magnitude + mSmoothingDown * lastMagnitude;
	}
	last = curr;
	return curr;
}

QVector<float> QAudioAnalyseInformant::getMagnitudeSpectrum()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	const std::vector<float>& curr = mGist->getMagnitudeSpectrum();
	QVector<float>& last = mSpectrumCache.MagnitudeSpectrum;
	QVector<float> result;
	last.resize(curr.size());
	result.resize(curr.size());
	for (int i = 0; i < curr.size(); i++) {
		const float magnitude = curr[i];
		const float lastMagnitude = last[i];
		if (magnitude > lastMagnitude)
			result[i] = (1. - mSmoothingUp) * magnitude + mSmoothingUp * lastMagnitude;
		else
			result[i] = (1. - mSmoothingDown) * magnitude + mSmoothingDown * lastMagnitude;
	}
	last = result;
	return result;
}

QVector<float> QAudioAnalyseInformant::getMelFrequencySpectrum()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	const std::vector<float>& curr = mGist->getMelFrequencySpectrum();
	QVector<float>& last = mSpectrumCache.MelFrequencySpectrum;
	QVector<float> result;
	last.resize(curr.size());
	result.resize(curr.size());
	for (int i = 0; i < curr.size(); i++) {
		const float magnitude = curr[i];
		const float lastMagnitude = last[i];
		if (magnitude > lastMagnitude)
			result[i] = (1. - mSmoothingUp) * magnitude + mSmoothingUp * lastMagnitude;
		else
			result[i] = (1. - mSmoothingDown) * magnitude + mSmoothingDown * lastMagnitude;
	}
	last = result;
	return result;
}

QVector<float> QAudioAnalyseInformant::getMelFrequencyCepstralCoefficients()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	const std::vector<float>& curr = mGist->getMelFrequencyCepstralCoefficients();
	QVector<float>& last = mSpectrumCache.MelFrequencyCepstralCoefficients;
	QVector<float> result;
	last.resize(curr.size());
	result.resize(curr.size());
	for (int i = 0; i < curr.size(); i++) {
		const float magnitude = curr[i];
		const float lastMagnitude = last[i];
		if (magnitude > lastMagnitude)
			result[i] = (1. - mSmoothingUp) * magnitude + mSmoothingUp * lastMagnitude;
		else
			result[i] = (1. - mSmoothingDown) * magnitude + mSmoothingDown * lastMagnitude;
	}
	last = result;
	return result;
}

float QAudioAnalyseInformant::getPeakEnergy()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->peakEnergy();
}

float QAudioAnalyseInformant::getRootMeanSquare()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->rootMeanSquare();
}

float QAudioAnalyseInformant::getZeroCrossingRate()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->zeroCrossingRate();
}

float QAudioAnalyseInformant::getSpectralCentroid()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->spectralCentroid();
}

float QAudioAnalyseInformant::getSpectralCrest()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->spectralCrest();
}

float QAudioAnalyseInformant::getSpectralFlatness()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->spectralFlatness();
}

float QAudioAnalyseInformant::getSpectralRolloff()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->spectralRolloff();
}

float QAudioAnalyseInformant::getSpectralKurtosis()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->spectralKurtosis();
}

float QAudioAnalyseInformant::getEnergyDifference()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->energyDifference();
}

float QAudioAnalyseInformant::getSpectralDifference()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->spectralDifference();
}

float QAudioAnalyseInformant::getSpectralDifferenceHWR()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->spectralDifferenceHWR();
}

float QAudioAnalyseInformant::getComplexSpectralDifference()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->complexSpectralDifference();
}

float QAudioAnalyseInformant::getHighFrequencyContent()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->highFrequencyContent();
}

float QAudioAnalyseInformant::getPitch()
{
	QMutexLocker<QMutex> locker(&QAudioAnalyseManager::Get().mMutex);
	return mGist->pitch();
}

QAudioAnalyseInformant::QAudioAnalyseInformant()
{
	QAudioAnalyseManager::Get().registerInformant(this);
}

QAudioAnalyseInformant::~QAudioAnalyseInformant()
{
	QAudioAnalyseManager::Get().unregisterInformant(this);
}

void QAudioAnalyseInformant::ensureGist()
{
	QAudioFormat currFormat = QAudioAnalyseManager::Get().getCurrentFormat();
	if (mGist == nullptr
		|| mGist->getAudioFrameSize() != mDesiredAudioFrameSize
		|| mGist->getSamplingFrequency() != currFormat.sampleRate()) {
		mGist = std::make_shared<Gist<float>>(mDesiredAudioFrameSize, currFormat.sampleRate());
	}
}

void QAudioAnalyseInformant::ensureCqt()
{
	QAudioFormat currFormat = QAudioAnalyseManager::Get().getCurrentFormat();
	if (!mCqt
		|| mCqt->getBinsPerOctave() != mDesiredCqtSetting.binsPerOctave
		|| mCqt->getOctaveNumber() != mDesiredCqtSetting.octaveNumber)
	{
		mCqt.reset(new Cqt::ConstantQTransform(mDesiredCqtSetting.binsPerOctave, mDesiredCqtSetting.octaveNumber));
		mCqt->init(mDesiredCqtSetting.hopSize);
		mCqt->initFs(currFormat.sampleRate(), mDesiredAudioFrameSize);
	}
}

void QAudioAnalyseInformant::process()
{
	ensureGist();
	std::vector<float> audioData = QAudioAnalyseManager::Get().popAudio(mGist->getAudioFrameSize());
	mGist->processAudioFrame(audioData);
	if (mDesiredCqtSetting.bEnabled) {
		ensureCqt();
		mCqt->inputBlock(audioData.data(), audioData.size());
	}
}
