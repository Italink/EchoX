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
	qDebug() <<"sdf " << mPlayGeomtry << singleArea;
	mPlayGeomtry |= singleArea;
	updateViewport();
	qDebug() << mPlayGeomtry << singleArea;
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
		it.key()->preSetup(it.value().process, graphBuilder);
		it.value().process += graphBuilder.getDeltaSec();
		if (it.value().process > it.key()->getPlayDurationSec()) {
			mOutdatedVFXList <<(it.key());
		}
	}
	//- QRect(452,-200 2200x1929)
	//--- QRhiViewport(bottom - left - x = 0 bottom - left - y = 129 width = 2200 height = 1800 minDepth = 0 maxDepth = 1)
	//--- QRhiViewport(bottom - left - x = 48 bottom - left - y = 0 width = 1400 height = 1400 minDepth = 0 maxDepth = 1)
	graphBuilder.addPass([this,rt = graphBuilder.getMainRenderTarget()](QRhiCommandBuffer* cmdBuffer) {
		qDebug() <<"-"<< mPlayGeomtry;
		cmdBuffer->beginPass(rt, QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f), { 1.0f, 0 });
		for (auto it = mVFXMap.begin(); it != mVFXMap.end(); it++) {
			qDebug() << "---" << it.value().viewport<<it.key()->mCachedPlayArea;
			it.key()->render(it.value().process, it.value().viewport, cmdBuffer);
		}
		cmdBuffer->endPass();
	});
}

void QWidgetVFXRenderer::endFrame()
{
	for (auto& item : mOutdatedVFXList) {
		mVFXMap.remove(item);
		Q_EMIT item->asFinished();
		item->setParent(nullptr);
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
			cacheRect.y() - mPlayGeomtry.y(),
			cacheRect.width(), 
			cacheRect.height());
	}
}

