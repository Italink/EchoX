#include "QWidgetVFXRenderer.h"
#include "Render/RenderGraph/QRenderGraphBuilder.h"

QWidgetVFXRenderer::QWidgetVFXRenderer()
	: IRenderer(QRhiHelper::InitParams({ QRhi::D3D12 }), QSize(800, 600), Type::Window)
{
}

void QWidgetVFXRenderer::addVFX(IWidgetVFX* inVFX)
{
	inVFX->setParent(this);
	mVFXMap.insert(inVFX, 0.0f);
}

void QWidgetVFXRenderer::setupGraph(QRenderGraphBuilder& graphBuilder)
{
	float delta = 0.01f;

	for (auto it = mVFXMap.begin(); it != mVFXMap.end(); it++) {
		it.key()->play(it.value(), graphBuilder);
		it.value() += delta;
		if (it.value() > it.key()->getPlayDurationSec()) {
			mOutdatedVFXList <<(it.key());
		}
	}
}

void QWidgetVFXRenderer::endFrame()
{
	for (auto& item : mOutdatedVFXList) {
		mVFXMap.remove(item);
		Q_EMIT item->asFinished();
		item->deleteLater();
	}

	if (!mOutdatedVFXList.isEmpty() && mVFXMap.isEmpty())
		Q_EMIT asEmptied();

	mOutdatedVFXList.clear();
}

