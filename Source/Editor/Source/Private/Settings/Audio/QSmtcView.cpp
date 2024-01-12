#include "QSmtcView.h"
#include <QGridLayout>
#include <QFile>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

#define ADD_LABEL(NAME, Col, Row) gLayout->addWidget(new QElideLabel(#NAME), Col, Row);  gLayout->addWidget(mLb##NAME, Col, Row + 1);

QSmtcView::QSmtcView()
	: mLbSourceAppId(new QElideLabel(""))
	, mLbSourceDeviceId(new QElideLabel(""))
	, mLbRenderDeviceId(new QElideLabel(""))
	, mLbMediaClassPrimaryID(new QElideLabel(""))
	, mLbTitle(new QElideLabel(""))
	, mLbArtist(new QElideLabel(""))
	, mLbSubtitle(new QElideLabel(""))
	, mLbTrackNumber(new QElideLabel(""))
	, mLbGenres(new QElideLabel(""))
	, mLbAlbumTitle(new QElideLabel(""))
	, mLbAlbumArtist(new QElideLabel(""))
	, mLbAlbumTrackCount(new QElideLabel(""))
	, mLbPlaybackState(new QElideLabel(""))
	, mLbPlaybackCaps(new QElideLabel(""))
	, mLbPlaybackMode(new QElideLabel(""))
	, mLbRepeatMode(new QElideLabel(""))
	, mLbLastPlayingFileTime(new QElideLabel(""))
	, mLbPlaybackRate(new QElideLabel(""))
	, mLbThumbnail(new QLabel())

{
	setStyleSheet("QWidget{color: #ffffff;}");

	QGridLayout* gLayout = new QGridLayout(this);
	gLayout->setColumnStretch(0, 3);
	gLayout->setColumnStretch(1, 3);
	gLayout->setColumnStretch(2, 5);
	gLayout->setColumnStretch(3, 5);
	gLayout->setVerticalSpacing(2);
	gLayout->setHorizontalSpacing(10);

	gLayout->addWidget(mLbThumbnail, 0, 0, 18, 2, Qt::AlignHCenter| Qt::AlignTop);
	ADD_LABEL(Title, 0, 2);
	ADD_LABEL(Artist, 1, 2);
	ADD_LABEL(Subtitle, 2, 2);
	ADD_LABEL(TrackNumber, 3, 2);
	ADD_LABEL(Genres, 4, 2);
	ADD_LABEL(AlbumTitle, 5, 2);
	ADD_LABEL(AlbumArtist, 6, 2);
	ADD_LABEL(AlbumTrackCount, 7, 2);

	ADD_LABEL(SourceAppId, 8, 2);
	ADD_LABEL(SourceDeviceId, 9, 2);
	ADD_LABEL(RenderDeviceId, 10, 2);
	ADD_LABEL(MediaClassPrimaryID, 11, 2);
	ADD_LABEL(PlaybackState, 12, 2);
	ADD_LABEL(PlaybackCaps, 13, 2);
	ADD_LABEL(PlaybackMode, 14, 2);
	ADD_LABEL(RepeatMode, 15, 2);
	ADD_LABEL(LastPlayingFileTime, 16, 2);
	ADD_LABEL(PlaybackRate, 17, 2);

	connect(&QSmtcManager::Get(), &QSmtcManager::asReceiveMediaPlaybackDataSourceInfo, this, &QSmtcView::updateDataSourceInfo);
	connect(&QSmtcManager::Get(), &QSmtcManager::asReceiveMediaPlaybackInfo, this, &QSmtcView::updatePlaybackInfo);
	connect(&QSmtcManager::Get(), &QSmtcManager::asMediaPlaybackDataChanged, this, []() {
		QSmtcManager::Get().requestGetMediaPlaybackDataSourceInfo();
		QSmtcManager::Get().requestGetMediaPlaybackInfo();
	});

	QSmtcManager::Get().requestGetMediaPlaybackDataSourceInfo();
	QSmtcManager::Get().requestGetMediaPlaybackInfo();
}

void QSmtcView::updateDataSourceInfo(QSmtcMediaPlaybackDataSourceInfo info)
{
	mLbSourceAppId->setDisplayText(info.SourceAppId);
	mLbSourceDeviceId->setDisplayText(info.SourceDeviceId);
	mLbRenderDeviceId->setDisplayText(info.RenderDeviceId);
	mLbMediaClassPrimaryID->setDisplayText(info.MediaClassPrimaryID);
	mLbTitle->setDisplayText(info.Title);
	mLbArtist->setDisplayText(info.Artist);
	mLbSubtitle->setDisplayText(info.Subtitle);
	mLbTrackNumber->setDisplayText(QString::number(info.TrackNumber));
	mLbGenres->setDisplayText(info.Genres.join(","));
	mLbAlbumTitle->setDisplayText(info.AlbumTitle);
	mLbAlbumArtist->setDisplayText(info.AlbumArtist);
	mLbAlbumTrackCount->setDisplayText(QString::number(info.AlbumTrackCount));
	if (QFile::exists(info.Thumbnail)) {
		mThumbnail = QImage(info.Thumbnail);
		mLbThumbnail->setPixmap(QPixmap::fromImage(mThumbnail.scaledToHeight(mLbThumbnail->height())));

		QT_BEGIN_NAMESPACE
			extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter * p, QImage & blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
		QT_END_NAMESPACE

		mThumbnailBlurred = QImage(mThumbnail.size(), QImage::Format_RGBA8888);
		mThumbnailBlurred.setDevicePixelRatio(mThumbnail.devicePixelRatioF());
		mThumbnailBlurred.fill(Qt::transparent);
		QPainter blurPainter(&mThumbnailBlurred);
		qt_blurImage(&blurPainter, mThumbnail, 30, false, false);
		blurPainter.end();
		update();
	}
}

void QSmtcView::updatePlaybackInfo(QSmtcMediaPlaybackInfo info)
{
	mLbPlaybackState->setDisplayText(info.PlaybackState);
	mLbPlaybackCaps->setDisplayText(info.PlaybackCaps);
	mLbPlaybackMode->setDisplayText(info.PlaybackMode);
	mLbRepeatMode->setDisplayText(info.RepeatMode);
	mLbLastPlayingFileTime->setDisplayText(info.LastPlayingFileTime);
	mLbPlaybackRate->setDisplayText(QString::number(info.PlaybackRate));
}

void QSmtcView::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	if (!mThumbnail.isNull()) {
		float wh = width() /(float) height();
		float imwh = mThumbnailBlurred.width() / (float)mThumbnailBlurred.height();
		QRect imgRect(0, 0, mThumbnailBlurred.width(), mThumbnailBlurred.height());
		if (imwh > wh) {
			imgRect.setHeight(height());
			imgRect.setWidth(height() * imwh);
		}
		else {
			imgRect.setHeight(width()/imwh);
			imgRect.setWidth(width());
		}
		imgRect.moveCenter(rect().center());
		painter.drawImage(imgRect, mThumbnailBlurred);
	}
	painter.fillRect(rect(), QColor(0, 0, 0, 100));
}

void QSmtcView::resizeEvent(QResizeEvent* e)
{

}
