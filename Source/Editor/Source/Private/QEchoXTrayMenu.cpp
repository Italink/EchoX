#include "QEchoXTrayMenu.h"
#include <QWidgetAction>
#include <QPainter>
#include <QStyleOption>

QEchoXMenu::QEchoXMenu(QWidget* parent)
	: QMenu(parent)
{
	setWindowFlag(Qt::FramelessWindowHint, true);
	setAttribute(Qt::WA_TranslucentBackground, true);
}

void QEchoXMenu::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	QStyleOption opt;
	opt.initFrom(this);
	QColor windowColor = opt.palette.color(QPalette::Window);
	windowColor.setAlphaF(0.95);
	painter.fillRect(rect(), windowColor);
	QMenu::paintEvent(e);
}
