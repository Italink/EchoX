#include "IWidgetVFX.h"

QRect IWidgetVFX::assessPlayArea()
{
	return QRect(mWidget->mapToGlobal(QPoint(0, 0)), mWidget->size());
}

void IWidgetVFX::setupWidget(QWidget* widget)
{
	mWidget = widget;
}

QWidget* IWidgetVFX::getWidget() const
{
	return mWidget;
}

