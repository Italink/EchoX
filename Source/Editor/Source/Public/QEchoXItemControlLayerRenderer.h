#ifndef QEchoXItemControlLayerRenderer_h__
#define QEchoXItemControlLayerRenderer_h__

#include "Render/IRenderer.h"
#include "EchoXEditorAPI.h"
#include "Render/RenderGraph/QRenderGraphBuilder.h"
#include "Render/RenderGraph/Painter/ImGuiPainter.h"

class ECHOXEDITOR_API QEchoXItemControlLayerRenderer : public IRenderer {
	Q_OBJECT
public:
	QEchoXItemControlLayerRenderer(QRhiHelper::InitParams params);
private:
	void setupGraph(QRenderGraphBuilder& graphBuilder) override;
	void renderControlLayer(ImGuiContext* Context);
private:
	QSharedPointer<ImGuiPainter> mImGuiPainter;
};

#endif // QEchoXItemControlLayerRenderer_h__
