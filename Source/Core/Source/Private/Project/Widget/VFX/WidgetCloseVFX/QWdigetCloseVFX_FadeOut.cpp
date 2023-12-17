#include "QWdigetCloseVFX_FadeOut.h"

QWdigetCloseVFX_FadeOut::QWdigetCloseVFX_FadeOut()
{
	mFadeOutFS = QRhiHelper::newShaderFromCode(QShader::FragmentStage, R"(#version 450
			layout (binding = 0) uniform sampler2D uWidgetImage;
			layout (binding = 1) uniform UniformBlock{
				float alpha;
			}UBO;
			layout (location = 0) in vec2 vUV;
			layout (location = 0) out vec4 outFragColor;
			void main() {
				outFragColor = vec4(texture(uWidgetImage, vUV).rgb, 1.0f) * UBO.alpha;
			}
	)");
}

float QWdigetCloseVFX_FadeOut::getPlayDurationSec() const
{
	return 1;
}

void QWdigetCloseVFX_FadeOut::play(float timeSec, QRenderGraphBuilder& builder)
{
	builder.setupSampler(mSampler, "Sampler",
		QRhiSampler::Linear,
		QRhiSampler::Linear,
		QRhiSampler::None,
		QRhiSampler::Repeat,
		QRhiSampler::Repeat,
		QRhiSampler::Repeat);

	builder.setupTexture(mTexture, "WidgetImage", QRhiTexture::RGBA8, getCachedPlayArea().size(), 1);
	builder.setupBuffer(mUniformBuffer, "WidgetAlpha", QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(float));

	builder.setupShaderResourceBindings(mBindings, "TextureBindings", {
		QRhiShaderResourceBinding::sampledTexture(0,QRhiShaderResourceBinding::FragmentStage,mTexture.get(),mSampler.get()),
		QRhiShaderResourceBinding::uniformBuffer(1,QRhiShaderResourceBinding::FragmentStage,mUniformBuffer.get())
		});

	QRhiGraphicsPipelineState PSO;

	QRhiGraphicsPipeline::TargetBlend blendState;
	blendState.enable = true;
	PSO.targetBlends = { blendState };

	PSO.sampleCount = builder.getMainRenderTarget()->sampleCount();
	PSO.shaderResourceBindings = mBindings.get();
	PSO.renderPassDesc = builder.getMainRenderTarget()->renderPassDescriptor();
	PSO.shaderStages = {
		{ QRhiShaderStage::Vertex, builder.getFullScreenVS() },
		{ QRhiShaderStage::Fragment, mFadeOutFS }
	};
	builder.setupGraphicsPipeline(mPipeline, "WidgetFadeOutPipeline", PSO);
	builder.addPass([this, &builder, timeSec](QRhiCommandBuffer* cmdBuffer) {
		QRhi* rhi = cmdBuffer->rhi();
		auto batch = rhi->nextResourceUpdateBatch();;
		if (mSigNeedUploadImage.ensure()) {
			batch->uploadTexture(mTexture.get(), mCahcedWidgetImage);
		}
		float process = 1.0 - timeSec / getPlayDurationSec();
		batch->updateDynamicBuffer(mUniformBuffer.get(), 0, sizeof(float), &process);
		cmdBuffer->beginPass(builder.getMainRenderTarget(), QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f), { 1.0f, 0 }, batch);
		cmdBuffer->setGraphicsPipeline(mPipeline.get());
		cmdBuffer->setViewport(QRhiViewport(0, 0, builder.getMainRenderTarget()->pixelSize().width(), builder.getMainRenderTarget()->pixelSize().height()));
		cmdBuffer->setShaderResources(mBindings.get());
		cmdBuffer->draw(4);
		cmdBuffer->endPass();
	});
}
