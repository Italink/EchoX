#include "QEchoXItemControlLayerRenderer.h"
#include "QEchoXController.h"

QEchoXItemControlLayerRenderer::QEchoXItemControlLayerRenderer(QRhiHelper::InitParams params)
	: IRenderer(params)
{
}

void QEchoXItemControlLayerRenderer::setupGraph(QRenderGraphBuilder& graphBuilder)
{
	if (!mImGuiPainter) {
		mImGuiPainter.reset(new ImGuiPainter);
		mImGuiPainter->setupWindow(maybeWindow());
		mImGuiPainter->setupPaintFunctor([this](ImGuiContext* Context) {
			this->renderControlLayer(Context);
		});
	}
	mImGuiPainter->setup(graphBuilder, graphBuilder.getMainRenderTarget());
	graphBuilder.addPass([this, rt = graphBuilder.getMainRenderTarget()](QRhiCommandBuffer* cmdBuffer) {
		QRhiResourceUpdateBatch* batch = cmdBuffer->rhi()->nextResourceUpdateBatch();
		mImGuiPainter->resourceUpdate(batch, cmdBuffer->rhi());
		cmdBuffer->beginPass(rt, QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f), { 1.0f, 0 }, batch);
		mImGuiPainter->paint(cmdBuffer, rt);
		cmdBuffer->endPass();
	});
}

void QEchoXItemControlLayerRenderer::renderControlLayer(ImGuiContext* Context)
{
	ImGui::SetCurrentContext(Context);
	ImGui::ShowDemoWindow();
	ImGuiIO& io = ImGui::GetIO();
	if (io.MouseDown[ImGuiMouseButton_Left]) {
		QEchoXController::Get()->requestActivateMainWindow();
		io.MouseDown[ImGuiMouseButton_Left] = false;
	}
}

