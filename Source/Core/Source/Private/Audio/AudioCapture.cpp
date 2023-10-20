#include "AudioCapture.h"
#include "AudioSink.h"
#include <windows.h>
#include <AudioClient.h>
#include <initguid.h>
#include <setupapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <future>
#include <thread>

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); }

class XWindowsAudioCapture : public IAudioCapture , public IMMNotificationClient{
private:
	IMMDeviceEnumerator* mOutputEnumerator = nullptr;
	std::atomic<bool> mRunning = false;
	std::promise<bool> mStoped;
	std::shared_ptr<std::thread> mCaptureThread;
public:
	bool start() override {
		if(!mSink){
			printf("Sink is nullptr.\n");
			return false;
		}

		CoInitialize(NULL);
		CoCreateInstance(__uuidof(MMDeviceEnumerator),
			nullptr,
			CLSCTX_ALL,
			__uuidof(IMMDeviceEnumerator),
			(void**)&mOutputEnumerator);
		HRESULT hr;
		hr = mOutputEnumerator->RegisterEndpointNotificationCallback(this);
		if (FAILED(hr)) {
			printf("Unable to activate audio client: %x.\n", hr);
			return false;
		}

		IMMDeviceCollection* pDeviceCollection = nullptr;
		IMMDevice* pDefaultDevice = nullptr;

		hr = mOutputEnumerator->GetDefaultAudioEndpoint(mType == Loudspeaker ? eRender : eCapture, eMultimedia, &pDefaultDevice);
		if (FAILED(hr)) {
			printf("Unable to activate audio client: %x.\n", hr);
			return false;
		}

		IPropertyStore* pProps = NULL;
		PROPVARIANT propVar;

		hr = pDefaultDevice->OpenPropertyStore(STGM_READ, &pProps);
		if (FAILED(hr)) {
			printf("Unable to activate audio client: %x.\n", hr);
			return false;
		}
		PropVariantInit(&propVar);
		hr = pProps->GetValue(PKEY_AudioEngine_DeviceFormat, &propVar);
		if (FAILED(hr)) {
			printf("Unable to activate audio client: %x.\n", hr);
			return false;
		}
		WAVEFORMATEX format = *(PWAVEFORMATEX)propVar.blob.pBlobData;
		format.cbSize = 0;
		format.wFormatTag = WAVE_FORMAT_PCM;

		SAFE_RELEASE(pProps);

		mStoped = std::promise<bool>();

		IAudioClient* _AudioClient;
		IAudioCaptureClient* _CaptureClient;
		HANDLE _AudioSamplesReadyEvent = NULL;
		hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		_AudioSamplesReadyEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
		if (_AudioSamplesReadyEvent == NULL) {
			printf("Unable to create samples ready event: %d.\n", GetLastError());
			return false;
		}

		hr = pDefaultDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&_AudioClient));
		if (FAILED(hr)) {
			printf("Unable to activate audio client: %x.\n", hr);
			return false;
		}

		size_t _FrameSize = (format.wBitsPerSample / 8) * format.nChannels;

		UINT32        _BufferSize;

		//Initialize Audio Engine
		hr = _AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
			(mType == Loudspeaker ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0) | AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST
			, 20 * 10000, 0, &format, NULL);

		XAudioFormat audioFormat;
		audioFormat.nChannels = format.nChannels;
		audioFormat.nSamplesPerSec = format.nSamplesPerSec;
		audioFormat.nAvgBytesPerSec = format.nAvgBytesPerSec;
		audioFormat.nBlockAlign = format.nBlockAlign;
		audioFormat.wBitsPerSample = format.wBitsPerSample;
		audioFormat.cbSize = format.cbSize;
		mSink->setFormat(audioFormat);

		if (FAILED(hr)) {
			printf("Unable to initialize audio client: %x.\n", hr);
			return false;
		}

		hr = _AudioClient->GetBufferSize(&_BufferSize);
		if (FAILED(hr)) {
			printf("Unable to get audio client buffer: %x. \n", hr);
			return false;
		}
		hr = _AudioClient->SetEventHandle(_AudioSamplesReadyEvent);
		if (FAILED(hr)) {
			printf("Unable to set ready event: %x.\n", hr);
			return false;
		}

		hr = _AudioClient->GetService(IID_PPV_ARGS(&_CaptureClient));
		if (FAILED(hr)) {
			printf("Unable to get new capture client: %x.\n", hr);
			return false;
		}

		hr = _AudioClient->Start();
		if (FAILED(hr)) {
			printf("Unable to get new capture client: %x.\n", hr);
			return false;
		}
		mRunning = true;

		mCaptureThread = std::make_shared<std::thread>([this, _AudioSamplesReadyEvent, _CaptureClient, _FrameSize, _AudioClient]() {
			while (mRunning) {
				DWORD waitResult = WaitForSingleObject(_AudioSamplesReadyEvent, INFINITE);
				BYTE* pData;
				UINT32 framesAvailable;
				DWORD  flags;
				HRESULT ret = _CaptureClient->GetBuffer(&pData, &framesAvailable, &flags, NULL, NULL);
				if (SUCCEEDED(ret)) {
					if (framesAvailable != 0) {
						if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
						}
						mSink->pushRawData(pData, framesAvailable * _FrameSize);
					}
					ret = _CaptureClient->ReleaseBuffer(framesAvailable);
					if (FAILED(ret)) {
						printf("Unable to release capture buffer: %x!\n", ret);
					}
				}
			}
			SAFE_RELEASE(_CaptureClient);
			SAFE_RELEASE(_AudioClient);
			mStoped.set_value_at_thread_exit(true);
			return true;
		});
		mCaptureThread->detach();
		return true;
	}
	bool stop() override {
		if (!mRunning) {
			return false;
		}
		mRunning = false;
		std::future<bool> future = mStoped.get_future();
		future.wait();
		return true;
	}
	bool isRunning() override {
		return mRunning;
	}
protected:
		ULONG STDMETHODCALLTYPE AddRef() { return 0; };
		ULONG STDMETHODCALLTYPE Release() { return 0; };
		HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, VOID**) {return 0;}
		HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(
			EDataFlow flow, ERole role,
			LPCWSTR pwstrDeviceId) {
			restart();
			return NULL;
		}
		HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId) {
			restart();
			return NULL;
		}
		HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId) {
			restart();
			return NULL;
		}
		HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) {
			restart();
			return NULL;
		}
		HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY){ return NULL; }
};

std::shared_ptr<IAudioCapture> IAudioCapture::Create()
{
	return std::make_shared<XWindowsAudioCapture>();
}

void IAudioCapture::setType(EType type)
{
	mType = type;
	if (isRunning()) {
		stop();
		start();
	}
}

void IAudioCapture::setSink(std::shared_ptr<XAudioSink> sink)
{
	mSink = sink;
	if (isRunning()) {
		stop();
		start();
	}
}

void IAudioCapture::restart()
{
	if (isRunning()) {
		stop();
	}
	start();
}

