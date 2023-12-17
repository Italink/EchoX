#include "QWidgetVFXRenderer.h"
#include "Render/RenderGraph/QRenderGraphBuilder.h"
#include "tracy/Tracy.hpp"

QWidgetVFXRenderer::QWidgetVFXRenderer(QRhiHelper::InitParams params)
	: IRenderer(params, QSize(800, 600), Type::Window)
{
}

void QWidgetVFXRenderer::addVFX(IWidgetVFX* inVFX)
{
	inVFX->setParent(this);
	mVFXMap.insert(inVFX, 0.0f);
}

void QWidgetVFXRenderer::setupGraph(QRenderGraphBuilder& graphBuilder)
{
	for (auto it = mVFXMap.begin(); it != mVFXMap.end(); it++) {
		ZoneScoped;
		ZoneText(it.key()->metaObject()->className(), strlen(it.key()->metaObject()->className()));
		it.key()->play(it.value(), graphBuilder);
		it.value() += graphBuilder.getDeltaSec();
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

