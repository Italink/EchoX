#ifndef AudioCapture_h__
#define AudioCapture_h__

#include <memory>
#include <windows.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <atomic>

class XAudioSink;

class IAudioCapture {
public:
	enum EType {
		Microphone,
		Loudspeaker
	};
	static std::shared_ptr<IAudioCapture> Create();
	void setType(EType type);
	void setSink(std::shared_ptr<XAudioSink> sink);
	virtual bool start() = 0;
	virtual bool stop() = 0;
	virtual bool isRunning() = 0;
	void restart();
protected:
	IAudioCapture() {}
protected:
	EType mType = Loudspeaker;
	std::shared_ptr<XAudioSink> mSink;
};

#endif // AudioCapture_h__
