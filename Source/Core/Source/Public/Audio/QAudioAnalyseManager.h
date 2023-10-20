#ifndef QAudioAnalyseManager_h__
#define QAudioAnalyseManager_h__

#include <QObject>
#include "EchoXCoreAPI.h"
#include <QAudioFormat>

class XAudioSink;
class IAudioCapture;
class QAudioAnalyseInformant;
template<typename _Ty> class Gist;

class ECHOXCORE_API QAudioAnalyseManager: public QObject
{
	Q_OBJECT
	friend class QAudioAnalyseInformant;
public:
	static QAudioAnalyseManager& Get();

	void startup();
	void shutdown();

	void tick();

	QAudioFormat getCurrentFormat();
	std::vector<float> popAudio(const size_t desiredSize);
private:
	QAudioAnalyseManager() {};
	void registerInformant(QAudioAnalyseInformant* informant);
	void unregisterInformant(QAudioAnalyseInformant* informant);
private:
	std::shared_ptr<XAudioSink> mAudioSink;
	std::shared_ptr<IAudioCapture> mAudioCapture;
	QList<QAudioAnalyseInformant*> mInformants;
};

class ECHOXCORE_API QAudioAnalyseInformant {
	friend class QAudioAnalyseManager;
public:
	enum WindowType
	{
		RectangularWindow,
		HanningWindow,
		HammingWindow,
		BlackmanWindow,
		TukeyWindow
	};
	void setAudioFrameSize(int v);
	void setWindowType(WindowType w);
private:
	QAudioAnalyseInformant();
	~QAudioAnalyseInformant();
	void process();
private:
	int mDesiredAudioFrameSize = 128;
	WindowType mDesiredWindowType = HanningWindow;
	std::shared_ptr<Gist<float>> mGist;
};

#endif // QAudioAnalyseManager_h__
