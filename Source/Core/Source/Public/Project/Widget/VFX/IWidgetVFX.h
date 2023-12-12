#ifndef IWidgetVFX_h__
#define IWidgetVFX_h__

#include <QWidget>
#include "Render/RenderGraph/QRenderGraphBuilder.h"

class IWidgetVFX: public QObject {
	Q_OBJECT
	friend class QWidgetVFXRenderer;
	friend class QWidgetVFXManager;
public:
	void setupWidget(QWidget* widget);
	QWidget* getWidget() const;
protected:
	virtual QRect assessPlayArea();
	virtual float getPlayDurationSec() const = 0;
	virtual void play(float progress, QRenderGraphBuilder& graphBuilder) = 0;
private:
	QWidget* mWidget = nullptr;
};

#endif // IWidgetVFX_h__
