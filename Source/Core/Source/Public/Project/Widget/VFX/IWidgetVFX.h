#ifndef IWidgetVFX_h__
#define IWidgetVFX_h__

#include <QWidget>
#include "Render/RenderGraph/QRenderGraphBuilder.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API IWidgetVFX: public QObject {
	Q_OBJECT
public:
	virtual float getPlayDurationSec() const = 0;
	QRect getCachedPlayArea() const;
Q_SIGNALS:
	void asStarted();
	void asFinished();
protected:
	virtual QRect assessWidget(QWidget* widget);
	virtual void play(float timeSec, QRenderGraphBuilder& graphBuilder) = 0;
protected:
	friend class QWidgetVFXRenderer;
	friend class QWidgetVFXManager;
	QRect mCachedPlayArea;
};

class ECHOXCORE_API IWidgetCloseVFX : public IWidgetVFX {
	Q_OBJECT
public:
	QRect assessWidget(QWidget* widget) override;
	QImage getCahceWidgetImage() const;
protected:
	QImage mCahcedWidgetImage;
	QRhiSignal mSigNeedUploadImage;
};


#endif // IWidgetVFX_h__
