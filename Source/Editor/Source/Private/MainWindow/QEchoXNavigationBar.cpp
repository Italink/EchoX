#include "QEchoXNavigationBar.h"
#include <QLabel>
#include <QPainter>
#include "QEchoXStyleSettings.h"

QEchoXNavigationItem::QEchoXNavigationItem(const QString& inName, QString inIconPath, QWidget* inPage)
	: mName(inName)
	, mIcon(new QSvgIcon(inIconPath, "Navigation", Qt::white))
	, mPage(inPage)
{
	setHoverColor(QColor(101, 104, 204, 150));
	setFixedHeight(40);
	mIcon->setColor(Qt::white);
	QLabel* lbIcon = new QLabel;
	lbIcon->setFixedSize(25,25);
	lbIcon->setPixmap(mIcon->getIcon().pixmap(QSize(20, 20)));
	QHBoxLayout* h = new QHBoxLayout(this);
	h->setContentsMargins(10, 0, 20, 0);
	h->addWidget(lbIcon);
	QLabel* lbName = new QLabel(inName);
	QPalette palette = lbName->palette();
	palette.setColor(QPalette::ColorRole::WindowText, Qt::white);
	lbName->setPalette(palette);
	h->addWidget(lbName);
}

void QEchoXNavigationItem::mousePressEvent(QMouseEvent* e)
{
	if (e->buttons() & Qt::LeftButton) {
		Q_EMIT asMousePressed(this);
	}
}

void QEchoXNavigationItem::paintEvent(QPaintEvent* event)
{
	if (mHoverd && mHoverEnabled) {
		QPainter painter(this);
		painter.fillRect(rect(), mHoverColor);
	}
}

QEchoXNavigationBar::QEchoXNavigationBar()
	: mLayout(new QVBoxLayout(this))
	, mLogo(new QSvgIcon(":/Resources/expand.png", "Navigation"))
	, mInnerShadow(":/Resources/inner_shadow.png")
{
	setFixedWidth(150);
	mAnimTimer.setInterval(15);
	connect(&mAnimTimer , &QTimer::timeout, this, [this]() {
		mAnimOffset += mAnimSpeed;
		if (qAbs(mAnimOffset - mCurrentItem->geometry().top()) <= qAbs(mAnimSpeed)) {
			mAnimOffset = mCurrentItem->geometry().top();
			mAnimTimer.stop();
		}
		update();
	});

	mLayout->setContentsMargins(0, 0, 0, 0);
	mLayout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
	QLabel* label = new QLabel;
	label->setMinimumHeight(35);
	label->setPixmap(mLogo->getIcon().pixmap(QSize(80, 35)));
	mLayout->addWidget(label);

}

QEchoXNavigationItem* QEchoXNavigationBar::addItem(const QString& inName, QString inIconPath, QWidget* inPage)
{
	QEchoXNavigationItem* item = new QEchoXNavigationItem(inName, inIconPath, inPage);
	connect(item, &QEchoXNavigationItem::asMousePressed, this, &QEchoXNavigationBar::setCurrentItem);
	item->setFixedWidth(width());
	mLayout->addWidget(item);
	return item;
}

void QEchoXNavigationBar::removeItem(QString inName)
{
	for (int i = 0; i < mLayout->count(); i++) {
		QEchoXNavigationItem* item = (QEchoXNavigationItem*)(mLayout->itemAt(i));
		if (item->getName() == inName) {
			mLayout->removeWidget(item);
			delete item;
			break;
		}
	}
}

void QEchoXNavigationBar::setCurrentItem(QEchoXNavigationItem* inItem)
{
	if (inItem != mCurrentItem) {
		mCurrentItem = inItem;
		if (mCurrentItem) {
			mAnimSpeed = (mCurrentItem->geometry().top() - mAnimOffset) / 5;
			mAnimTimer.start();
		}
		Q_EMIT asCurrentItemChanged(mCurrentItem);
		update();
	}
}

void QEchoXNavigationBar::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.fillRect(rect().adjusted(-1, -1, 1, 1), QEchoXStyleSettings::Get()->getThemeColor());
	if (mCurrentItem) {
		QRect itemGeom = mCurrentItem->geometry();
		itemGeom.moveTop(mAnimOffset);
		const int WhiteLineWidth = 4;
		painter.fillRect(QRect(itemGeom.width() - WhiteLineWidth, itemGeom.y(), WhiteLineWidth, itemGeom.height()), Qt::white);
		painter.setOpacity(0.7);
		painter.fillRect(itemGeom.adjusted(0, 0, -WhiteLineWidth, 0), mInnerShadow);
	}
}
