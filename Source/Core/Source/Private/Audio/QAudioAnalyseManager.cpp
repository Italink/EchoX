#include "QAudioAnalyseManager.h"
#include "AudioCapture.h"
#include "AudioSink.h"
#include "Gist.h"

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
	mTickTimer.setInterval(30);
	mTickTimer.moveToThread(&mTickThread);
	connect(&mTickThread, SIGNAL(started()), &mTickTimer, SLOT(start()));
	connect(&mTickTimer, &QTimer::timeout, this, &QAudioAnalyseManager::tick, Qt::DirectConnection);
	
	mAudioCapture->start();
	mTickThread.start();
}

void QAudioAnalyseManager::shutdown()
{
	Q_ASSERT(mInformants.isEmpty());
	mAudioCapture->stop();
	mAudioCapture.reset();
	mAudioSink.reset();
}

void QAudioAnalyseManager::tick()
{
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
	return informant;
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

std::vector<float> QAudioAnalyseInformant::getMagnitudeSpectrum()
{
	return mGist->getMagnitudeSpectrum();
}

std::vector<float> QAudioAnalyseInformant::getMelFrequencySpectrum()
{
	return mGist->getMelFrequencySpectrum();
}

std::vector<float> QAudioAnalyseInformant::getMelFrequencyCepstralCoefficients()
{
	return mGist->getMelFrequencyCepstralCoefficients();
}

float QAudioAnalyseInformant::getPeakEnergy()
{
	return mGist->peakEnergy();
}

float QAudioAnalyseInformant::getRootMeanSquare()
{
	return mGist->rootMeanSquare();
}

float QAudioAnalyseInformant::getZeroCrossingRate()
{
	return mGist->zeroCrossingRate();
}

float QAudioAnalyseInformant::getSpectralCentroid()
{
	return mGist->spectralCentroid();
}

float QAudioAnalyseInformant::getSpectralCrest()
{
	return mGist->spectralCrest();
}

float QAudioAnalyseInformant::getSpectralFlatness()
{
	return mGist->spectralFlatness();
}

float QAudioAnalyseInformant::getSpectralRolloff()
{
	return mGist->spectralRolloff();
}

float QAudioAnalyseInformant::getSpectralKurtosis()
{
	return mGist->spectralKurtosis();
}

float QAudioAnalyseInformant::getEnergyDifference()
{
	return mGist->energyDifference();
}

float QAudioAnalyseInformant::getSpectralDifference()
{
	return mGist->spectralDifference();
}

float QAudioAnalyseInformant::getSpectralDifferenceHWR()
{
	return mGist->spectralDifferenceHWR();
}

float QAudioAnalyseInformant::getComplexSpectralDifference()
{
	return mGist->complexSpectralDifference();
}

float QAudioAnalyseInformant::getHighFrequencyContent()
{
	return mGist->highFrequencyContent();
}

float QAudioAnalyseInformant::getPitch()
{
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

void QAudioAnalyseInformant::process()
{
	ensureGist();
	mGist->processAudioFrame(QAudioAnalyseManager::Get().popAudio(mGist->getAudioFrameSize()));
}
