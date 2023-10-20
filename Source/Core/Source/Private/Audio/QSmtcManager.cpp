#include "QSmtcManager.h"
#include <QJsonDocument>

QSmtcManager::QSmtcManager()
{
}

QSmtcManager& QSmtcManager::Get()
{
	static QSmtcManager Instance;
	return Instance;
}

void QSmtcManager::startup()
{
	ensureConnect();
}

void QSmtcManager::shutdown()
{
	if (mClient) {
		mClient->disconnectFromHost();
		mClient->waitForDisconnected();
	}
	mServerProcess.kill();
}

bool QSmtcManager::ensureConnect()
{
	if (!mClient) {
		mClient = new QTcpSocket();
	}
	if(!mServerProcess.isOpen()){
		mServerProcess.setProgram("QSmtcServer.exe");
		mServerProcess.setProcessChannelMode(QProcess::SeparateChannels);
		mServerProcess.start();
		mServerProcess.waitForStarted();

		qDebug() << mServerProcess.state() << mServerProcess.processChannelMode() << mServerProcess.readAllStandardError();
	}
	if (mClient->state() != QAbstractSocket::ConnectedState) {
		mClient->connectToHost(QHostAddress::LocalHost, 40529);
		mClient->waitForConnected();
		connect(mClient, &QTcpSocket::readyRead, this, &QSmtcManager::notify);
	}

#if 1
	QTimer* timer = new QTimer;
	connect(timer, &QTimer::timeout, [this]() {
		requestSendCommand(QSmtcCommand::Play);
		requestGetMediaPlaybackDataSourceInfo();
		requestGetMediaTimelineProperties();
		qDebug() << mServerProcess.state() << mClient->state() << mServerProcess.readAllStandardError() << mServerProcess.readAllStandardError();
	});
	timer->setInterval(1000);
	timer->start();
#endif
	return true;
}

void QSmtcManager::requestSendCommand(QSmtcCommand command)
{
	QJsonObject json;
	json["Command"] = "SendCommand";
	json["CommandID"] = (int)command;
	sendMessage(json);
}

void QSmtcManager::requestGetMediaPlaybackDataSourceInfo()
{
	QJsonObject json;
	json["Command"] = "GetMediaPlaybackDataSourceInfo";
	sendMessage(json);
}

void QSmtcManager::requestGetMediaPlaybackInfo()
{
	QJsonObject json;
	json["Command"] = "GetMediaPlaybackInfo";
	sendMessage(json);
}

void QSmtcManager::requestGetMediaTimelineProperties()
{
	QJsonObject json;
	json["Command"] = "GetMediaTimelineProperties";
	sendMessage(json);
}

void QSmtcManager::sendMessage(QJsonObject json)
{
	QByteArray data = QJsonDocument(json).toJson();
	int length = data.size();
	mClient->write((char*)&length, 4);
	mClient->write(data);
}

void QSmtcManager::notify()
{
	int dataLength = 0;
	mClient->read((char*)&dataLength, 4);
	if (dataLength <= 0)
		return;
	QByteArray data = mClient->read(dataLength);
	QJsonObject json = QJsonDocument::fromJson(data).object();
	QString reply = json.value("Reply").toString();
	if (reply == "MediaPlaybackDataChanged") {
		Q_EMIT asMediaPlaybackDataChanged();
	}
	else if (reply == "MediaPlaybackDataSourceInfo") {
		QSmtcMediaPlaybackDataSourceInfo info;
		info.SourceAppId = json["SourceAppId"].toString();
		info.SourceDeviceId = json["SourceDeviceId"].toString();
		info.RenderDeviceId = json["RenderDeviceId"].toString();
		info.MediaClassPrimaryID = json["MediaClassPrimaryID"].toString();
		info.Title = json["Title"].toString();
		info.Artist = json["Artist"].toString();
		info.Subtitle = json["Subtitle"].toString();
		info.Genres = json["Genres"].toString().split("|",Qt::SplitBehaviorFlags::SkipEmptyParts);
		info.TrackNumber = json["TrackNumber"].toInt();
		info.AlbumTitle = json["AlbumTitle"].toString();
		info.AlbumArtist = json["AlbumArtist"].toString();
		info.AlbumTrackCount = json["AlbumTrackCount"].toInt();
		Q_EMIT asReceiveMediaPlaybackDataSourceInfo(info);
	}
	else if (reply == "MediaPlaybackInfo") {
		QSmtcMediaPlaybackInfo info;
		info.PlaybackState = json["PlaybackState"].toString();
		info.PlaybackCaps = json["PlaybackCaps"].toString();
		info.PlaybackMode = json["PlaybackMode"].toString();
		info.PlaybackRate = json["PlaybackRate"].toDouble();
		info.RepeatMode = json["RepeatMode"].toString();
		info.LastPlayingFileTime = json["LastPlayingFileTime"].toString();
		Q_EMIT asReceiveMediaPlaybackInfo(info);
	}
	else if (reply == "MediaTimelineProperties") {
		QSmtcMediaTimelineProperties info;
		info.Position = QTime::fromString(json["Position"].toString());
		info.StartTime = QTime::fromString(json["StartTime"].toString());
		info.EndTime = QTime::fromString(json["EndTime"].toString());
		info.MinSeekTime = QTime::fromString(json["MinSeekTime"].toString());
		info.MaxSeekTime = QTime::fromString(json["MaxSeekTime"].toString());
		info.PositionSetFileTime = QTime::fromString(json["PositionSetFileTime"].toString());
		Q_EMIT asReceiveMediaTimelineProperties(info);
	}
	qDebug() << json;
}
