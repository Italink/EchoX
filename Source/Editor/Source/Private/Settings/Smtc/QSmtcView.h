#ifndef QSmtcView_h__
#define QSmtcView_h__

#include <QWidget>
#include <QLabel>
#include "Audio/QSmtcManager.h"
#include "Widgets/QElideLabel.h"

class QSmtcView: public QWidget {
	Q_OBJECT
	using QWidget::QWidget;
public:
	QSmtcView();
private:
	QElideLabel* mLbSourceAppId;
	QElideLabel* mLbSourceDeviceId;
	QElideLabel* mLbRenderDeviceId;
	QElideLabel* mLbMediaClassPrimaryID;
	QElideLabel* mLbTitle;
	QElideLabel* mLbArtist;
	QElideLabel* mLbSubtitle;
	QElideLabel* mLbTrackNumber;
	QElideLabel* mLbGenres;
	QElideLabel* mLbAlbumTitle;
	QElideLabel* mLbAlbumArtist;
	QElideLabel* mLbAlbumTrackCount;

	QElideLabel* mLbPlaybackState;
	QElideLabel* mLbPlaybackCaps;
	QElideLabel* mLbPlaybackMode;
	QElideLabel* mLbRepeatMode;
	QElideLabel* mLbLastPlayingFileTime;
	QElideLabel* mLbPlaybackRate;

	QLabel* mLbThumbnail;
	QImage mThumbnail;
	QImage mThumbnailBlurred;
private:
	void updateDataSourceInfo(QSmtcMediaPlaybackDataSourceInfo info);
	void updatePlaybackInfo(QSmtcMediaPlaybackInfo info);

	void paintEvent(QPaintEvent* e) override;
	void resizeEvent(QResizeEvent* e) override;
};


#endif // QSmtcView_h__
