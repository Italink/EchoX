#include "QWindow3DEffect.h"
#include "private/qvulkandefaultinstance_p.h"
#include <QPainter>
#include <QVulkanInstance>
#include "qvulkaninstance.h"
#include "QWindow3D.h"
#include <private/qwidgetwindow_p.h>
#include <private/qwidgetrepaintmanager_p.h>
#include <qpaintengine.h>

QWindow3DEffect::QWindow3DEffect(QObject* parent)
	: QGraphicsEffect(parent)
{
}

void QWindow3DEffect::setupRhi(QRhi* rhi)
{
	mRhi = rhi;
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
	PixmapPadMode mode = PadToEffectiveBoundingRect;
	QPoint pos;
	QPixmap widgetImage = sourcePixmap(Qt::DeviceCoordinates, &pos, mode);
	widgetImage = widgetImage.copy(mWidget->rect());
	if (mRenderTargetTexture.isNull() || widgetImage.size() != mRenderTargetTexture->pixelSize()) {
		mWidgetTexutre.reset(mRhi->newTexture(QRhiTexture::RGBA8, widgetImage.size(), 1, QRhiTexture::UsedAsTransferSource));
		mWidgetTexutre->create();
		mRenderTargetTexture.reset(mRhi->newTexture(QRhiTexture::RGBA8, mContainter->size(), 1, QRhiTexture::RenderTarget | QRhiTexture::UsedAsTransferSource));
		mRenderTargetTexture->create();
		mRenderTarget.reset(mRhi->newTextureRenderTarget({ mRenderTargetTexture.get() }));
		mRenderTargetDesc.reset(mRenderTarget->newCompatibleRenderPassDescriptor());
		mRenderTarget->setRenderPassDescriptor(mRenderTargetDesc.get());
		mRenderTarget->create();
		mPainter.reset(new QWindow3DPainter);
		mPainter->setupRhi(mRhi);
		mPainter->setupRenderPassDesc(mRenderTargetDesc.get());
		mPainter->setupTexture(mWidgetTexutre.get());
		mPainter->compile();
	}
	QWidgetPrivate* w = QWidgetPrivate::get(mWidget);
	QRhiCommandBuffer* cmdBuffer;
	if (mRhi->beginOffscreenFrame(&cmdBuffer) != QRhi::FrameOpSuccess)
		return;
	auto resourceUpdates = mRhi->nextResourceUpdateBatch();
	resourceUpdates->uploadTexture(mWidgetTexutre.get(), widgetImage.toImage().convertedTo(QImage::Format_RGBA8888));

	mPainter->setupNDCQuad(mContainter->getLoaclNDCQuad());
	mPainter->updateResource(resourceUpdates);

	const QColor clearColor = QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f);
	const QRhiDepthStencilClearValue dsClearValue = { 1.0f,0 };
	cmdBuffer->beginPass(mRenderTarget.get(), clearColor, dsClearValue, resourceUpdates);
	mPainter->paint(cmdBuffer, mRenderTarget.get());
	cmdBuffer->endPass();

	resourceUpdates = mRhi->nextResourceUpdateBatch();
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
	mRhi->endOffscreenFrame();
}
