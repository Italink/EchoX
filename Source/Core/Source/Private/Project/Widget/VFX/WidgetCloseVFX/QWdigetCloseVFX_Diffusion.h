#ifndef QWdigetCloseVFX_Diffusion_h__
#define QWdigetCloseVFX_Diffusion_h__

#include "IWidgetVFX.h"

class QWidgetCloseVFX_Diffusion : public IWidgetCloseVFX {
public:
	QShader mDiffusionInitCS;
	QShader mDiffusionUpdateCS;
	QShader mDiffusionRenderVS;
	QShader mDiffusionRenderFS;
	struct Vertex {
		QVector2D position;
		QVector2D velocity;
		QVector2D uv;
		float age;
		float padding;
	};
	struct UpdateContextBuffer {
		QVector2D playAreaSize;
		QVector2D imageSize;
		float deltaSec;
		float process;
	};
	QRhiBufferRef mVertexBuffer;
	QRhiBufferRef mUniformBuffer;
	QRhiSamplerRef mSampler;
	QRhiTextureRef mWidgetTexture;
	QRhiShaderResourceBindingsRef mInitBindings;
	QRhiComputePipelineRef mInitPipeline;
	QRhiShaderResourceBindingsRef mUpdateBindings;
	QRhiComputePipelineRef mUpdatePipepline;
	QRhiShaderResourceBindingsRef mRenderBindings;
	QRhiGraphicsPipelineRef mRenderPipeline;
public:
	QWidgetCloseVFX_Diffusion();
	QRect assessWidget(QWidget* widget) override;
	float getPlayDurationSec() const  override;

	void preSetup(float timeSec, QRenderGraphBuilder& builder) override;
	void render(float timeSec, const QRhiViewport& viewport, QRhiCommandBuffer* cmdBuffer) override;
};

#endif // QWdigetCloseVFX_Diffusion_h__
