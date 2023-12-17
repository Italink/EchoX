#include "QWdigetCloseVFX_Diffusion.h"

QWidgetCloseVFX_Diffusion::QWidgetCloseVFX_Diffusion()
{
	mDiffusionInitCS = QRhiHelper::newShaderFromCode(QShader::ComputeStage, R"(#version 450
		struct Particle {
			vec2 position;
			vec2 velocity;
			vec2 uv;
			float age;
		};
		layout(std140,binding = 0) buffer ParticleBuffer{
			Particle Particles[];
		};
		layout (binding = 1) uniform UniformBlock{
			vec2 playAreaSize;
			vec2 imageSize;
			float deltaSec;
			float process;
		}UBO;
		#define CurrID gl_GlobalInvocationID.y * UBO.imageSize.x + gl_GlobalInvocationID.x
		#define CurrParticle Particles[int(CurrID)]
		void main() {
			CurrParticle.position = (vec2(gl_GlobalInvocationID.xy) / UBO.imageSize * 2 - 1) *  UBO.imageSize / UBO.playAreaSize;
			CurrParticle.velocity = vec2(0);
			CurrParticle.uv = vec2(gl_GlobalInvocationID.xy/UBO.imageSize);
			CurrParticle.age = 0;
		}
	)");

	mDiffusionUpdateCS = QRhiHelper::newShaderFromCode(QShader::ComputeStage, R"(#version 450
		struct Particle {
			vec2 position;
			vec2 velocity;
			vec2 uv;
			float age;
		};
		layout(std140,binding = 0) buffer ParticleBuffer{
			Particle Particles[];
		};
		layout (binding = 1) uniform UniformBlock{
			vec2 playAreaSize;
			vec2 imageSize;
			float deltaSec;
			float process;
		}UBO;
		#define CurrID gl_GlobalInvocationID.y * UBO.imageSize.x + gl_GlobalInvocationID.x
		#define CurrParticle Particles[int(CurrID)]
		float rand(vec2 seed, float min, float max){
			return min + (max-min) * fract(sin(dot(seed * 0.4536456145213 - vec2(1.1231251,1102.1651) ,vec2(12.9898,78.233))) * 43758.5453);
		}
		void main() {
			CurrParticle.position = CurrParticle.position + CurrParticle.velocity * UBO.deltaSec;
			float realProcess = UBO.process / 0.7;
			vec2 randomVelocity = CurrParticle.uv.x - CurrParticle.uv.y > 1 - realProcess* 2 ? vec2(rand(CurrParticle.uv,0.0,0.0001),rand(CurrParticle.uv * UBO.deltaSec * 63.4165  ,-0.003,0.01)) + rand(CurrParticle.uv,0.0,0.001)* sin( CurrParticle.uv.y/CurrParticle.uv.x * 10)   : vec2(0);
			CurrParticle.velocity = CurrParticle.velocity + randomVelocity;
			CurrParticle.age = CurrParticle.age + (CurrParticle.uv.x - CurrParticle.uv.y > 1 - realProcess * 2 ? UBO.deltaSec : 0 );
		}
	)");

	mDiffusionRenderVS = QRhiHelper::newShaderFromCode(QShader::VertexStage, R"(#version 450
		layout(location = 0) in vec2 inPosition;
		layout(location = 1) in vec2 inVelocity;
		layout(location = 2) in vec2 inUV;
		layout(location = 3) in float inAge;
		layout(location = 0) out vec2 vUV;
		layout(location = 1) out float vAlpha;
		out gl_PerVertex { 
			vec4 gl_Position; 
			float gl_PointSize;
		};
		void main() {
			vUV = inUV;
			const float lifetime = 1;
			vAlpha =   clamp(lifetime - inAge,0,1)/lifetime;
			gl_Position = vec4(inPosition,0,1);
			gl_PointSize = 1;
		}
	)");

	mDiffusionRenderFS = QRhiHelper::newShaderFromCode(QShader::FragmentStage, R"(#version 450
		layout(location = 0) in vec2 vUV;
		layout(location = 1) in float vAlpha;
		layout (location = 0) out vec4 outFragColor;
		layout (binding = 0) uniform sampler2D uWidgetTexture;
		layout (binding = 1) uniform UniformBlock{
			vec2 playAreaSize;
			vec2 imageSize;
			float deltaSec;
			float process;
		}UBO;
		void main() {
			outFragColor = vec4(texture(uWidgetTexture, vUV).rgb, 1) * vAlpha;
		}
	)");
}

QRect QWidgetCloseVFX_Diffusion::assessWidget(QWidget* widget)
{
	return IWidgetCloseVFX::assessWidget(widget).adjusted(-500,-500, 500, 500);
}

float QWidgetCloseVFX_Diffusion::getPlayDurationSec() const
{
	return 5;
}

void QWidgetCloseVFX_Diffusion::play(float timeSec, QRenderGraphBuilder& builder)
{
	builder.setupBuffer(mVertexBuffer, "Vertex", QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer | QRhiBuffer::StorageBuffer, mCahcedWidgetImage.width() * mCahcedWidgetImage.height() * sizeof(Vertex));
	builder.setupBuffer(mUniformBuffer, "WidgetAlpha", QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(UpdateContextBuffer));
	builder.setupTexture(mWidgetTexture, "WidgetTexture", QRhiTexture::RGBA8, mCahcedWidgetImage.size(), 1);
	builder.setupSampler(mSampler, "Sampler",
		QRhiSampler::Linear,
		QRhiSampler::Linear,
		QRhiSampler::None,
		QRhiSampler::Repeat,
		QRhiSampler::Repeat,
		QRhiSampler::Repeat);

	builder.setupShaderResourceBindings(mInitBindings, "InitBindings", {
		QRhiShaderResourceBinding::bufferLoadStore(0, QRhiShaderResourceBinding::ComputeStage, mVertexBuffer.get()),
		QRhiShaderResourceBinding::uniformBuffer(1,QRhiShaderResourceBinding::ComputeStage,mUniformBuffer.get())
	});
	QRhiComputePipelineState CPSO;
	CPSO.shaderResourceBindings = mInitBindings.get();
	CPSO.shaderStage = { QRhiShaderStage::Compute, mDiffusionInitCS };
	builder.setupComputePipeline(mInitPipeline, "DiffusionInit", CPSO);

	builder.setupShaderResourceBindings(mUpdateBindings, "UpdateBindings", {
			QRhiShaderResourceBinding::bufferLoadStore(0, QRhiShaderResourceBinding::ComputeStage, mVertexBuffer.get()),
			QRhiShaderResourceBinding::uniformBuffer(1,QRhiShaderResourceBinding::ComputeStage,mUniformBuffer.get())
	});
	CPSO.shaderResourceBindings = mUpdateBindings.get();
	CPSO.shaderStage = { QRhiShaderStage::Compute, mDiffusionUpdateCS };
	builder.setupComputePipeline(mUpdatePipepline, "DiffusionUpdate", CPSO);


	builder.setupShaderResourceBindings(mRenderBindings, "TextureBindings", {
		QRhiShaderResourceBinding::sampledTexture(0,QRhiShaderResourceBinding::FragmentStage,mWidgetTexture.get(),mSampler.get()),
		QRhiShaderResourceBinding::uniformBuffer(1,QRhiShaderResourceBinding::FragmentStage,mUniformBuffer.get())
	});

	QRhiGraphicsPipelineState PSO;
	PSO.topology = QRhiGraphicsPipeline::Points;
	QRhiGraphicsPipeline::TargetBlend blendState;
	blendState.enable = true;
	PSO.targetBlends = { blendState };
	PSO.sampleCount = builder.getMainRenderTarget()->sampleCount();
	PSO.shaderResourceBindings = mRenderBindings.get();
	PSO.renderPassDesc = builder.getMainRenderTarget()->renderPassDescriptor();
	PSO.shaderStages = {
		{ QRhiShaderStage::Vertex, mDiffusionRenderVS},
		{ QRhiShaderStage::Fragment, mDiffusionRenderFS }
	};
	PSO.vertexInputLayout.setBindings({ QRhiVertexInputBinding(sizeof(Vertex)) });
	PSO.vertexInputLayout.setAttributes({
		QRhiVertexInputAttribute{ 0, 0, QRhiVertexInputAttribute::Float2, offsetof(Vertex, position) },
		QRhiVertexInputAttribute{ 0, 1, QRhiVertexInputAttribute::Float2, offsetof(Vertex, velocity) },
		QRhiVertexInputAttribute{ 0, 2, QRhiVertexInputAttribute::Float2, offsetof(Vertex, uv) },
		QRhiVertexInputAttribute{ 0, 3, QRhiVertexInputAttribute::Float, offsetof(Vertex, age) },
	});
	builder.setupGraphicsPipeline(mRenderPipeline, "WidgetDiffusionPipeline", PSO);
	builder.addPass([this, &builder, timeSec](QRhiCommandBuffer* cmdBuffer) {
		QRhi* rhi = cmdBuffer->rhi();
		auto batch = rhi->nextResourceUpdateBatch();
		bool bNeedInit = false;
		if (mSigNeedUploadImage.ensure()) {
			batch->uploadTexture(mWidgetTexture.get(), mCahcedWidgetImage);
			bNeedInit = true;
		}
		UpdateContextBuffer ctx;
		ctx.playAreaSize = QVector2D(mCachedPlayArea.width(), mCachedPlayArea.height());
		ctx.imageSize = QVector2D(mCahcedWidgetImage.width(), mCahcedWidgetImage.height());
		ctx.process = timeSec / getPlayDurationSec();
		ctx.deltaSec = builder.getDeltaSec();
		//qDebug() << "process" << ctx.process;
		//qDebug() << "deltaSec" << builder.getDeltaSec();
		batch->updateDynamicBuffer(mUniformBuffer.get(), 0, sizeof(UpdateContextBuffer), &ctx);
		cmdBuffer->resourceUpdate(batch);

		if (bNeedInit) {
			cmdBuffer->beginComputePass();
			cmdBuffer->setComputePipeline(mInitPipeline.get());
			cmdBuffer->setShaderResources(mInitBindings.get());
			cmdBuffer->dispatch(mCahcedWidgetImage.width(), mCahcedWidgetImage.height(), 1);
			cmdBuffer->endComputePass();
		}

		cmdBuffer->beginComputePass();
		cmdBuffer->setComputePipeline(mUpdatePipepline.get());
		cmdBuffer->setShaderResources(mUpdateBindings.get());
		cmdBuffer->dispatch(mCahcedWidgetImage.width(), mCahcedWidgetImage.height(), 1);
		cmdBuffer->endComputePass();

		cmdBuffer->beginPass(builder.getMainRenderTarget(), QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f), { 1.0f, 0 });
		cmdBuffer->setGraphicsPipeline(mRenderPipeline.get());
		cmdBuffer->setViewport(QRhiViewport(0, 0, builder.getMainRenderTarget()->pixelSize().width(), builder.getMainRenderTarget()->pixelSize().height()));
		cmdBuffer->setShaderResources(mRenderBindings.get());
		const QRhiCommandBuffer::VertexInput VertexInput(mVertexBuffer.get(), 0);
		cmdBuffer->setVertexInput(0, 1, &VertexInput);
		cmdBuffer->draw(mCahcedWidgetImage.width() * mCahcedWidgetImage.height());
		cmdBuffer->endPass();
	});
}
