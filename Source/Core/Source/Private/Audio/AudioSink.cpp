#include "AudioSink.h"
#include <corecrt_memory.h>

const unsigned int PCMS8MaxAmplitude = 128;
const unsigned short PCMS16MaxAmplitude = 32768; // because minimum is -32768
const unsigned int PCMS24MaxAmplitude = 8388608;
const unsigned int PCMS32MaxAmplitude = 2147483648;

XAudioSink::XAudioSink()
{
}

void XAudioSink::setFormat(XAudioFormat format)
{
	mFormat = format;
}

void XAudioSink::pushRawData(unsigned char* data, const size_t size)
{
	if (mBufferSize + size >= MAX_BUFFER_SIZE) {
		size_t moveSize = mCacheKeepSize * (mFormat.wBitsPerSample / 8) * mFormat.nChannels;
		memcpy_s(mBuffer, moveSize, mBuffer + mBufferSize - moveSize, moveSize);
		mBufferSize = moveSize;
	}
	memcpy_s(mBuffer + mBufferSize, size, data, size);
	mBufferSize += size;
}

std::vector<float> XAudioSink::popAudio(const size_t size) const
{
	std::vector<float> audioData(size);
	int offset = mBufferSize - size * mFormat.wBitsPerSample/8 * mFormat.nChannels;
	if (offset < 0) {
		return audioData;
	}
	unsigned const char* offsetPtr = mBuffer + offset;
	const int step = mFormat.wBitsPerSample / 8 * mFormat.nChannels;
	if (mFormat.wBitsPerSample == 8) {
		for (int i = 0; i < size; i++) {
			audioData[i] = (*reinterpret_cast<const char*>(offsetPtr)) / double(PCMS8MaxAmplitude);
			offsetPtr += step;
		}
	}
	else if(mFormat.wBitsPerSample == 16){
		for (int i = 0; i < size; i++) {
			audioData[i] = (*reinterpret_cast<const int16_t*>(offsetPtr)) / double(PCMS16MaxAmplitude);
			offsetPtr += step;
		}
	}
	return audioData;
}

