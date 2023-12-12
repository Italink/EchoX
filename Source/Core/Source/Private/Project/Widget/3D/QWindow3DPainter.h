#ifndef QWindow3DPainter_h__
#define QWindow3DPainter_h__

#include "rhi/qrhi.h"
#include "QQuadF.h"

class QWindow3DPainter {
public:
	QWindow3DPainter();

	void setupRhi(QRhi* inRhi) { mRhi = inRhi;}
	void setupRenderPassDesc(QRhiRenderPassDescriptor* desc) { mRenderPassDesc = desc;}
	void setupSampleCount(int sampleCount) { mSampleCount = sampleCount;}

	void setupNDCQuad(QQuadF quad);
	QQuadF getNDCQuad() {return mNDCQuad; }

	void setupTexture(QRhiTexture* texture);
	QRhiTexture* getTexture() { return mTexture; }

	void compile();
	void updateResource(QRhiResourceUpdateBatch* batch);
	void paint(QRhiCommandBuffer* cmdBuffer, QRhiRenderTarget* renderTarget);
protected:
	QRhi* mRhi;
	QQuadF mNDCQuad;
	bool bNeedUpdate = false;
	int mSampleCount;
	QRhiTexture* mTexture = nullptr;
	QRhiRenderPassDescriptor* mRenderPassDesc;
	QScopedPointer<QRhiBuffer> mVertexBuffer;
	QScopedPointer<QRhiBuffer> mIndexBuffer;
	QScopedPointer<QRhiBuffer> mUniformBuffer;
	QScopedPointer<QRhiSampler> mSampler;
	QRhiSampler::Filter mMagFilter = QRhiSampler::Linear;
	QRhiSampler::Filter mMinFilter = QRhiSampler::Linear;
	QRhiSampler::Filter mMipmapMode = QRhiSampler::None;
	QRhiSampler::AddressMode mAddressU = QRhiSampler::Repeat;
	QRhiSampler::AddressMode mAddressV = QRhiSampler::Repeat;
	QRhiSampler::AddressMode mAddressW = QRhiSampler::Repeat;
	QScopedPointer<QRhiGraphicsPipeline> mPipeline;
	QScopedPointer<QRhiShaderResourceBindings> mBindings;
};

#endif // QWindow3DPainter_h__
