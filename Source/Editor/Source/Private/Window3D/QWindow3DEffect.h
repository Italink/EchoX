#ifndef QWindow3DEffect_h__
#define QWindow3DEffect_h__

#include <QGraphicsEffect>
#include "QWindow3DPainter.h"
#include <rhi/qrhi.h>

class QWindow3D;

class QWindow3DEffect: public QGraphicsEffect {
public:
	QWindow3DEffect(QObject * parent = nullptr);
	void setupRhi(QRhi* rhi);
	void setupWidget(QWindow3D* containter, QWidget* widget);
protected:
	QRectF boundingRectFor(const QRectF& rect) const override;
	void draw(QPainter* painter) override;
private:
	QWidget* mWidget = nullptr;
	QWindow3D* mContainter = nullptr;
	QRhi* mRhi = nullptr;
	QScopedPointer<QWindow3DPainter> mPainter;
	QScopedPointer<QRhiTexture> mWidgetTexutre;
	QScopedPointer<QRhiTexture> mRenderTargetTexture;
	QScopedPointer<QRhiTextureRenderTarget> mRenderTarget;
	QScopedPointer<QRhiRenderPassDescriptor> mRenderTargetDesc;
};

#endif // QWindow3DEffect_h__
