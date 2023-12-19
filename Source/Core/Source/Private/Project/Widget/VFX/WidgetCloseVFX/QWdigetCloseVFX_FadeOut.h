#ifndef QWdigetCloseVFX_FadeOut_h__
#define QWdigetCloseVFX_FadeOut_h__

#include "IWidgetVFX.h"

class QWdigetCloseVFX_FadeOut : public IWidgetCloseVFX {
public:
	QShader mFadeOutFS;
	QRhiBufferRef mUniformBuffer;
	QRhiSamplerRef mSampler;
	QRhiTextureRef mTexture;
	QRhiGraphicsPipelineRef mPipeline;
	QRhiShaderResourceBindingsRef mBindings;
public:
	QWdigetCloseVFX_FadeOut();
	float getPlayDurationSec() const  override;

	void preSetup(float timeSec, QRenderGraphBuilder& builder) override;
	void render(float timeSec, const QRhiViewport& viewport, QRhiCommandBuffer* cmdBuffer) override;
};

#endif // QWdigetCloseVFX_FadeOut_h__