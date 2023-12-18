#ifndef QWidgetVFXRenderer_h__
#define QWidgetVFXRenderer_h__

#include "Render/IRenderer.h"
#include "IWidgetVFX.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QWidgetVFXRenderer : public IRenderer {
	Q_OBJECT
public:
	QWidgetVFXRenderer(QRhiHelper::InitParams params);
	QRect addVFX(QWidget* inWidget, IWidgetVFX* inVFX);
Q_SIGNALS:
	void asEmptied();
private:
	void setupGraph(QRenderGraphBuilder& graphBuilder) override;
	void endFrame() override;
	void updateViewport();
private:
	struct VFXState {
		QRhiViewport viewport;
		float playtimeSec = 0.0f;
		bool bPending = true;
	};
	QRect mPlayGeomtry;
	QMap<IWidgetVFX*, VFXState> mVFXMap;
	QList<IWidgetVFX*> mOutdatedVFXList;
};

#endif // QWidgetVFXRenderer_h__
