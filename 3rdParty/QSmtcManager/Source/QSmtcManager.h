#ifndef QSmtcManager_h__
#define QSmtcManager_h__

#include <QObject>
#include <QJsonObject>
#include <QTcpSocket>
#include <QProcess>
#include <QTimer>

enum class QSmtcCommand {
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

struct QSmtcMediaPlaybackDataSourceInfo
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

struct QSmtcMediaPlaybackInfo {
	QString PlaybackState;
	QString PlaybackCaps;
	QString PlaybackMode;
	QString RepeatMode;
	QString LastPlayingFileTime;
	double PlaybackRate;
};

struct QSmtcMediaTimelineProperties {
	QTime Position;
	QTime StartTime;
	QTime EndTime;
	QTime MinSeekTime;
	QTime MaxSeekTime;
	QTime PositionSetFileTime;
};

class QSmtcManager: public QObject{
	Q_OBJECT

	struct SourceInfo {
		QString title;
	};
public:
	QSmtcManager();
	~QSmtcManager();

	bool ensureConnect();
	void requestSendCommand(QSmtcCommand command);
	void requestGetMediaPlaybackDataSourceInfo();
	void requestGetMediaPlaybackInfo();
	void requestGetMediaTimelineProperties();
private:
	void sendMessage(QJsonObject json);
	void notify();
Q_SIGNALS:
	void asMediaPlaybackDataChanged();
	void asReceiveMediaPlaybackDataSourceInfo(QSmtcMediaPlaybackDataSourceInfo);
	void asReceiveMediaPlaybackInfo(QSmtcMediaPlaybackInfo);
	void asReceiveMediaTimelineProperties(QSmtcMediaTimelineProperties);
public:
	QProcess mServerProcess;
	QTcpSocket* mClient = nullptr;
};

#endif // QSmtcManager_h__
