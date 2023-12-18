#include "QWidgetVFXRenderer.h"
#include "Render/RenderGraph/QRenderGraphBuilder.h"
#include "tracy/Tracy.hpp"

QWidgetVFXRenderer::QWidgetVFXRenderer(QRhiHelper::InitParams params)
	: IRenderer(params, QSize(800, 600), Type::Window)
{
}

QRect QWidgetVFXRenderer::addVFX(QWidget* inWidget, IWidgetVFX* inVFX)
{
	QRect singleArea = inVFX->assessWidget(inWidget);
	inVFX->setParent(this);
	inVFX->mCachedPlayArea = singleArea;
	VFXState state;
	state.bPending = true;
	mVFXMap.insert(inVFX, state);
	mPlayGeomtry |= singleArea;
	updateViewport();
	return mPlayGeomtry;
}

void QWidgetVFXRenderer::setupGraph(QRenderGraphBuilder& graphBuilder)
{
	for (auto it = mVFXMap.begin(); it != mVFXMap.end(); it++) {
		ZoneScoped;
		ZoneText(it.key()->metaObject()->className(), strlen(it.key()->metaObject()->className()));
		if (it->bPending) {
			Q_EMIT it.key()->asStarted();
			it->bPending = false;
		}
		it.key()->preSetup(it.value().playtimeSec, graphBuilder);
		it.value().playtimeSec += graphBuilder.getDeltaSec();
		if (it.value().playtimeSec > it.key()->getPlayDurationSec()) {
			mOutdatedVFXList <<(it.key());
		}
	}
	graphBuilder.addPass([this, rt = graphBuilder.getMainRenderTarget()](QRhiCommandBuffer* cmdBuffer) {
		cmdBuffer->beginPass(rt, QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f), { 1.0f, 0 });
		for (auto it = mVFXMap.begin(); it != mVFXMap.end(); it++) {
			it.key()->render(it.value().playtimeSec, it.value().viewport, cmdBuffer);
		}
		cmdBuffer->endPass();
	});
}

void QWidgetVFXRenderer::endFrame()
{
	for (auto& item : mOutdatedVFXList) {
		mVFXMap.remove(item);
		Q_EMIT item->asFinished();
		item->deleteLater();
	}
	if (!mOutdatedVFXList.isEmpty() && mVFXMap.isEmpty()) {
		Q_EMIT asEmptied();
		mPlayGeomtry = QRect();
	}
	mOutdatedVFXList.clear();
}

void QWidgetVFXRenderer::updateViewport()
{
	for (auto it = mVFXMap.begin(); it != mVFXMap.end(); it++) {
		const QRect& cacheRect = it.key()->mCachedPlayArea;
		it->viewport.setViewport(
			cacheRect.x() - mPlayGeomtry.x(),
			mPlayGeomtry.bottomLeft().y()- cacheRect.bottomLeft().y(),
			cacheRect.width(), 
			cacheRect.height());
	}
}

