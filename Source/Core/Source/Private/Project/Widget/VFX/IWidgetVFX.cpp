#include "IWidgetVFX.h"
#include "Object/QEngineObjectManager.h"

QRect IWidgetVFX::assessWidget(QWidget* widget)
{
	return QRect(widget->mapToGlobal(QPoint(0, 0)), widget->size());
}

QRect IWidgetVFX::getCachedPlayArea() const
{
	return mCachedPlayArea;
}

QRect IWidgetCloseVFX::assessWidget(QWidget* widget)
{
	mCahcedWidgetImage = widget->grab().toImage().convertedTo(QImage::Format_RGBA8888);
	mSigNeedUploadImage.request();
	return IWidgetVFX::assessWidget(widget);
}

QImage IWidgetCloseVFX::getCahceWidgetImage() const
{
	return mCahcedWidgetImage;
}

QENGINE_REGISTER_CLASS(IWidgetCloseVFX)
