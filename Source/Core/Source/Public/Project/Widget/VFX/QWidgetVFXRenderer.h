#ifndef QWidgetVFXRenderer_h__
#define QWidgetVFXRenderer_h__

#include "Render/IRenderer.h"
#include "IWidgetVFX.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QWidgetVFXRenderer : public IRenderer {
	Q_OBJECT
public:
	QWidgetVFXRenderer();
	void addVFX(IWidgetVFX* inVFX);
private:
	void setupGraph(QRenderGraphBuilder& graphBuilder) override;
private:
	QMap<IWidgetVFX*, float> mVFXProgress;
};

#endif // QWidgetVFXRenderer_h__
