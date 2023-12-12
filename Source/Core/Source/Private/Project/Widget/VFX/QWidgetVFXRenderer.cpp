#include "QWidgetVFXRenderer.h"
#include "Render/RenderGraph/QRenderGraphBuilder.h"

QWidgetVFXRenderer::QWidgetVFXRenderer()
	: IRenderer(QRhiHelper::InitParams({ QRhi::D3D12 }), QSize(800, 600), Type::Window)
{

}

void QWidgetVFXRenderer::addVFX(IWidgetVFX* inVFX)
{
	inVFX->setParent(this);
	mVFXProgress.insert(inVFX, 0.0f);
}

void QWidgetVFXRenderer::setupGraph(QRenderGraphBuilder& graphBuilder)
{
	float delta = 0.1f;

	for (auto it = mVFXProgress.begin(); it != mVFXProgress.end(); it++) {
		it.key()->play(it.value(), graphBuilder);
		it.value() += delta;
	}

	//for (const auto& VFX : mVFXProgress.asKeyValueRange()) {
	//	if (VFX.second > VFX.first->getPlayDurationSec()) {
	//		mVFXProgress.remove(VFX.first);
	//	}
	//}
}

