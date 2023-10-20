#ifndef QSmtcManager_h__
#define QSmtcManager_h__

#include <QObject>
#include <QJsonObject>
#include <QTcpSocket>
#include <QProcess>
#include <QTimer>
#include "EchoXCoreAPI.h"

enum class ECHOXCORE_API QSmtcCommand {
	Play = 0,
	Pause,
	Stop,
	Record,
	FastForward,
	Rewind,
	Next,
	Previous,
	ChannelUp,
	ChannelDown,
	PlayPauseToggle,
	Max
};

struct ECHOXCORE_API QSmtcMediaPlaybackDataSourceInfo
{
	QString SourceAppId;
	QString SourceDeviceId;
	QString RenderDeviceId;
	QString MediaClassPrimaryID;
	QString Title;
	QString Artist;
	QString Subtitle;
	int TrackNumber;
	QStringList Genres;
	QString AlbumTitle;
	QString AlbumArtist;
	int AlbumTrackCount;
};

struct ECHOXCORE_API QSmtcMediaPlaybackInfo {
	QString PlaybackState;
	QString PlaybackCaps;
	QString PlaybackMode;
	QString RepeatMode;
	QString LastPlayingFileTime;
	double PlaybackRate;
};

struct ECHOXCORE_API QSmtcMediaTimelineProperties {
	QTime Position;
	QTime StartTime;
	QTime EndTime;
	QTime MinSeekTime;
	QTime MaxSeekTime;
	QTime PositionSetFileTime;
};

class ECHOXCORE_API QSmtcManager: public QObject{
	Q_OBJECT
public:
	static QSmtcManager& Get();

	void startup();
	void shutdown();

	void requestSendCommand(QSmtcCommand command);
	void requestGetMediaPlaybackDataSourceInfo();
	void requestGetMediaPlaybackInfo();
	void requestGetMediaTimelineProperties();
private:
	void sendMessage(QJsonObject json);
	void notify();
	bool ensureConnect();
Q_SIGNALS:
	void asMediaPlaybackDataChanged();
	void asReceiveMediaPlaybackDataSourceInfo(QSmtcMediaPlaybackDataSourceInfo);
	void asReceiveMediaPlaybackInfo(QSmtcMediaPlaybackInfo);
	void asReceiveMediaTimelineProperties(QSmtcMediaTimelineProperties);
public:
	QSmtcManager();
	QProcess mServerProcess;
	QTcpSocket* mClient = nullptr;
};

#endif // QSmtcManager_h__
