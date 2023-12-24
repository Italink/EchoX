#include "QEchoXStyleSettings.h"
#include "QEngineEditorStyleManager.h"
#include "QEchoXApplication.h"
#include "MainWindow/QEchoXMainWindow.h"

QEchoXStyleSettings::QEchoXStyleSettings()
{
	refreshDetailViewStyle();
}

QColor QEchoXStyleSettings::getThemeColor()
{
	return mThemeColor;
}

void QEchoXStyleSettings::setThemeColor(QColor var)
{
	if (mThemeColor != var) {
		mThemeColor = var;
		refreshDetailViewStyle();
		Q_EMIT asStyleChanged();
	}
}

QColor QEchoXStyleSettings::getHoveredColor()
{
	return mDetailViewPalette.mHoveredColor;
}

void QEchoXStyleSettings::setHoveredColor(QColor var)
{
	if (mDetailViewPalette.mHoveredColor != var) {
		mDetailViewPalette.mHoveredColor = var;
		refreshDetailViewStyle();
		Q_EMIT asStyleChanged();
	}
}

QColor QEchoXStyleSettings::getSelectedColor()
{
	return mDetailViewPalette.mSelectedColor;
}

void QEchoXStyleSettings::setSelectedColor(QColor var)
{
	if (mDetailViewPalette.mSelectedColor != var) {
		mDetailViewPalette.mSelectedColor = var;
		refreshDetailViewStyle();
		Q_EMIT asStyleChanged();
	}
}

float QEchoXStyleSettings::getShadowWidth()
{
	return mShadowWidth;
}

void QEchoXStyleSettings::setShadowWidth(float var)
{
	if (mShadowWidth != var) {
		mShadowWidth = var; 
		Q_EMIT asStyleChanged();
	}
}

void QEchoXStyleSettings::refreshDetailViewStyle()
{
	QColor backgroundColor = QColor(255, 255, 255);
	QColor fontColor = QColor(50, 50, 50);
	QColor inlineFontColor = QColor(50, 50, 50);
	if (mThemeColor.valueF() < 0.6) {
		inlineFontColor = QColor(255, 255, 255);
	}
	mDetailViewPalette.mShadowColor = QColor(200, 200, 200);
	mDetailViewPalette.mGridLineColor = QColor(240, 240, 240);
	mDetailViewPalette.mCategoryColor = QColor(250, 250, 250, 100);
	mDetailViewPalette.mHoveredColor = mThemeColor.lighter(100);
	mDetailViewPalette.mHoveredColor.setAlpha(150);
	mDetailViewPalette.mSelectedColor = mThemeColor.lighter(100);
	mDetailViewPalette.mSelectedColor.setAlpha(200);
	mDetailViewPalette.mArrowColor = mThemeColor;
	mDetailViewPalette.mIconColor = mThemeColor;

	mDetailViewPalette.mStyleSheet = QString(R"(
		QElideLabel{
			color:%1;
		}
		QWidget{
			color:%1;
			border:none;
		}
		QLabel{
			background-color: transparent;
		}
		QDetailView, QDetailView * {
			font-size: 8pt;
			background-color: %2;
		}
		QtColorDialog,QNotificationBlock{
			border: 1px solid %3;
		}
		QHoverWidget{
			background-color: %2;
			qproperty-HoverColor: %3; 
		}
		QSplitter::handle { background-color: %4; }
		QLineEdit,QTextEdit{
			background-color: transparent;
			color:%1;
			border: 0px;
		}
		QPushButton,QComboBox {
			background-color: %3;
			border: 0px; 
			color: %5;
			padding: 2px 5px 2px 5px; 
			border-radius: 3px; 
		}
		QPushButton:hover,QComboBox:hover,QPushButton:checked {
			background-color: %6;
		}
		QComboBox::drop-down {
			subcontrol-origin: padding;
			subcontrol-position: top right;
			width: 15px;
			border:none;
		}
		QComboBox::down-arrow {
			image: url(:/Resources/expand.png);
			height:15px;
			width:15px;
		}
		QPushButton:pressed,QComboBox:pressed { 
			background-color: %3;
		}
		QComboBox QAbstractItemView {
			padding: 0px 0px 4px 0px;
			border: 0px solid transparent;
			border-radius: 0px;
			color: %1;
			background-color: %2;
			selection-background-color: %3; 
		}
		QComboBox QAbstractItemView::item{
			height:20px;
			background-color:%2;
		}
		QComboBox QAbstractItemView::item:hover{  
		  background-color: %3;
		  color:#ffffff;
		}
		QComboBox QAbstractItemView::item:selected{
		  background-color: %6;
		  color:#ffffff;
		}
		QScrollBar:vertical,
		QScrollBar:horizontal  {
			width: 8px;
			background: %2;
		}
		QScrollBar::tryDo:vertical,
		QScrollBar::tryDo:horizontal {
			background:  %3;
			min-height: 30px;
		}
		QScrollBar::tryDo:vertical:hover,
		QScrollBar::tryDo:horizontal:hover {
			background: %3;
		}
		QScrollBar::sub-line:vertical, QScrollBar::add-line:vertical,
		QScrollBar::sub-line:horizontal, QScrollBar::add-line:horizontal {
			width: 0;
			height: 0;
		}
		QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical,
		QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
			background: none;
		}
		QScrollBar::handle:vertical {
			background: %3;
			min-height: 20px;
			border-radius: 2px;
			border: none;
		}
		)")
		.arg(fontColor.name(QColor::HexArgb))
		.arg(backgroundColor.name(QColor::HexArgb))
		.arg(mDetailViewPalette.mHoveredColor.name(QColor::HexArgb))
		.arg(mDetailViewPalette.mGridLineColor.name(QColor::HexArgb))
		.arg(inlineFontColor.name(QColor::HexArgb))
		.arg(mDetailViewPalette.mSelectedColor.name(QColor::HexArgb))
		.toLocal8Bit();
	QEngineEditorStyleManager::Instance()->setPalette(mDetailViewPalette);
}

QByteArray QEchoXStyleSettings::getStyleSheet() const
{
	return mDetailViewPalette.mStyleSheet;
}

