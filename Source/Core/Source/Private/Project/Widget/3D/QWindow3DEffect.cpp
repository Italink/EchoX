#include "QWindow3DEffect.h"
#include "private/qvulkandefaultinstance_p.h"
#include <QPainter>
#include <QVulkanInstance>
#include <QApplication>
#include "QWindow3D.h"
#include <private/qwidgetwindow_p.h>
#include <private/qwidgetrepaintmanager_p.h>
#include <qpaintengine.h>
#include "QEchoXCoreApplication.h"

QWindow3DEffect::QWindow3DEffect(QObject* parent)
	: QGraphicsEffect(parent)
{
}

void QWindow3DEffect::setupWidget(QWindow3D* containter, QWidget* widget)
{
	mContainter = containter;
	mWidget = widget;
}

QRectF QWindow3DEffect::boundingRectFor(const QRectF& rect) const{
	return mContainter->rect();
}

void QWindow3DEffect::draw(QPainter* painter){
	QRhi* rhi = static_cast<QEchoXCoreApplication*>(QCoreApplication::instance())->getGlobalRhi();
	PixmapPadMode mode = PadToEffectiveBoundingRect;
	QPoint pos;
	QPixmap widgetImage = sourcePixmap(Qt::DeviceCoordinates, &pos, mode);
	QRect dpiRect(mWidget->x() * mWidget->devicePixelRatioF(), mWidget->y() * mWidget->devicePixelRatioF(), mWidget->width() * mWidget->devicePixelRatioF(), mWidget->height() * mWidget->devicePixelRatioF());
	widgetImage = widgetImage.copy(dpiRect);
	if (mRenderTargetTexture.isNull() || widgetImage.size() != mRenderTargetTexture->pixelSize()) {
		mWidgetTexutre.reset(rhi->newTexture(QRhiTexture::RGBA8, widgetImage.size(), 1, QRhiTexture::UsedAsTransferSource));
		mWidgetTexutre->create();
		mRenderTargetTexture.reset(rhi->newTexture(QRhiTexture::RGBA8, mContainter->size(), 1, QRhiTexture::RenderTarget | QRhiTexture::UsedAsTransferSource));
		mRenderTargetTexture->create();
		mRenderTarget.reset(rhi->newTextureRenderTarget({ mRenderTargetTexture.get() }));
		mRenderTargetDesc.reset(mRenderTarget->newCompatibleRenderPassDescriptor());
		mRenderTarget->setRenderPassDescriptor(mRenderTargetDesc.get());
		mRenderTarget->create();
		mPainter.reset(new QWindow3DPainter);
		mPainter->setupRhi(rhi);
		mPainter->setupSampleCount(mRenderTarget->sampleCount());
		mPainter->setupRenderPassDesc(mRenderTargetDesc.get());
		mPainter->setupTexture(mWidgetTexutre.get());
		mPainter->compile();
	}
	QWidgetPrivate* w = QWidgetPrivate::get(mWidget);
	QRhiCommandBuffer* cmdBuffer;
	if (rhi->beginOffscreenFrame(&cmdBuffer) != QRhi::FrameOpSuccess)
		return;
	auto resourceUpdates = rhi->nextResourceUpdateBatch();
	resourceUpdates->uploadTexture(mWidgetTexutre.get(), widgetImage.toImage().convertedTo(QImage::Format_RGBA8888));
	widgetImage.toImage().convertedTo(QImage::Format_RGBA8888).save("ased.png");
	mPainter->setupNDCQuad(mContainter->getLoaclNDCQuad());
	mPainter->updateResource(resourceUpdates);

	const QColor clearColor = QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f);
	const QRhiDepthStencilClearValue dsClearValue = { 1.0f,0 };
	cmdBuffer->beginPass(mRenderTarget.get(), clearColor, dsClearValue, resourceUpdates);
	mPainter->paint(cmdBuffer, mRenderTarget.get());
	cmdBuffer->endPass();

	resourceUpdates = rhi->nextResourceUpdateBatch();
	QRhiReadbackResult rbResult;

	rbResult.completed = [&rbResult, pos, painter] {
		if (!rbResult.data.isEmpty()) {
			const uchar* p = reinterpret_cast<const uchar*>(rbResult.data.constData());
			QImage image(p, rbResult.pixelSize.width(), rbResult.pixelSize.height(), QImage::Format_RGBA8888);
			painter->drawImage(pos, image);
		}
	};
	QRhiReadbackDescription rb(mRenderTargetTexture.get());
	resourceUpdates->readBackTexture(rb, &rbResult);
	cmdBuffer->resourceUpdate(resourceUpdates);
	rhi->endOffscreenFrame();
}
