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
}

void QAudioAnalyseManager::shutdown()
{
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

void QAudioAnalyseInformant::setWindowType(WindowType w)
{
	mDesiredWindowType = w;
}

QAudioAnalyseInformant::QAudioAnalyseInformant()
{
	QAudioAnalyseManager::Get().registerInformant(this);
}

QAudioAnalyseInformant::~QAudioAnalyseInformant()
{
	QAudioAnalyseManager::Get().unregisterInformant(this);
}

void QAudioAnalyseInformant::process()
{
	QAudioFormat currFormat = QAudioAnalyseManager::Get().getCurrentFormat();
	if (mGist == nullptr
		|| mGist->getAudioFrameSize() != mDesiredAudioFrameSize
		|| mGist->getSamplingFrequency() != currFormat.sampleRate()) {
		mGist = std::make_shared<Gist<float>>(mDesiredAudioFrameSize, currFormat.sampleRate());
	}
	mGist->processAudioFrame(QAudioAnalyseManager::Get().popAudio(mGist->getAudioFrameSize()));
}
