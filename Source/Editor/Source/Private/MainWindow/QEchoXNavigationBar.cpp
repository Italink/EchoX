#include "QEchoXNavigationBar.h"
#include <QLabel>
#include <QPushButton>
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
{
	setFixedWidth(150);
	mLayout->setContentsMargins(0, 0, 0, 0);
	mLayout->setAlignment(Qt::AlignTop|Qt::AlignLeft);

	QLabel* label = new QLabel;
	label->setMinimumHeight(35);
	label->setPixmap(mLogo->getIcon().pixmap(QSize(80, 35)));
	mLayout->addWidget(label);
}

void QEchoXNavigationBar::addItem(const QString& inName, QString inIconPath, QWidget* inPage)
{
	QEchoXNavigationItem* item = new QEchoXNavigationItem(inName, inIconPath, inPage);
	item->setFixedWidth(width() - mShadowWidth);
	mLayout->addWidget(item);
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

void QEchoXNavigationBar::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.fillRect(rect().adjusted(-1, -1, -mShadowWidth, 1), QEchoXStyleSettings::Get()->getThemeColor());
	QColor shadowColor = QColor(150, 150, 150);
	QRect shadowRect(width() - mShadowWidth, 0 , mShadowWidth, height());
	QLinearGradient shadowLinearGradient;
	shadowLinearGradient.setStart(shadowRect.topLeft());
	shadowLinearGradient.setFinalStop(shadowRect.topRight());
	shadowLinearGradient.setColorAt(0, QColor(shadowColor.red(), shadowColor.green(), shadowColor.blue()));
	shadowLinearGradient.setColorAt(1, QColor(shadowColor.red(), shadowColor.green(), shadowColor.blue(), 0));
	painter.fillRect(shadowRect, shadowLinearGradient);
}
