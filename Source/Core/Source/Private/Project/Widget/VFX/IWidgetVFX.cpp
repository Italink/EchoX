#include "IWidgetVFX.h"

QRect IWidgetVFX::assessWidget(QWidget* widget)
{
	return QRect(widget->mapToGlobal(QPoint(0, 0)), widget->size());
}

QRect IWidgetVFX::getCachedPlayArea() const
{
	return mCachedPlayArea;
}

