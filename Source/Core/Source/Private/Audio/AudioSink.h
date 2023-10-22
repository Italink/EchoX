#ifndef AudioSink_h__
#define AudioSink_h__

#include <vector>

struct XAudioFormat {
	unsigned int nChannels = 0;      /* number of channels (i.e. mono, stereo...) */
	unsigned int nSamplesPerSec;     /* sample rate */
	unsigned int nAvgBytesPerSec;    /* for buffer estimation */
	unsigned int nBlockAlign;        /* block size of data */
	unsigned int wBitsPerSample;     /* number of bits per sample of mono data */
	unsigned int cbSize;             /* the count in bytes of the size of */
};

const int MAX_BUFFER_SIZE = 1000000;

class XAudioSink {
public:
	XAudioSink();
	const XAudioFormat& format(){ return mFormat;}
	void setFormat(XAudioFormat format);
	void pushRawData(unsigned char* data, const size_t size) ;
	std::vector<float> popAudio(const size_t size, int channelIndex = 0) const;
private:
	XAudioFormat mFormat;
	unsigned char mBuffer[MAX_BUFFER_SIZE] ={};
	size_t mBufferSize = 0;
	size_t mCacheKeepSize = 1 << 13;
};

#endif // AudioSink_h__
