#ifndef IWidgetVFX_h__
#define IWidgetVFX_h__

#include <QWidget>
#include "Render/RenderGraph/QRenderGraphBuilder.h"

class IWidgetVFX: public QObject {
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
private:
	friend class QWidgetVFXRenderer;
	friend class QWidgetVFXManager;
	QRect mCachedPlayArea;
};

#endif // IWidgetVFX_h__
