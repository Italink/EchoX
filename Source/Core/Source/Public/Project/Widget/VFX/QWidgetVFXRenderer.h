#ifndef QWidgetVFXRenderer_h__
#define QWidgetVFXRenderer_h__

#include "Render/IRenderer.h"
#include "IWidgetVFX.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QWidgetVFXRenderer : public IRenderer {
	Q_OBJECT
public:
	QWidgetVFXRenderer(QRhiHelper::InitParams params);
	void addVFX(IWidgetVFX* inVFX);
Q_SIGNALS:
	void asEmptied();
private:
	void setupGraph(QRenderGraphBuilder& graphBuilder) override;
	void endFrame() override;
private:
	QMap<IWidgetVFX*, float> mVFXMap;
	QList<IWidgetVFX*> mOutdatedVFXList;
};

#endif // QWidgetVFXRenderer_h__
