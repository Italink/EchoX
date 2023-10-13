#include "QWindow3DPainter.h"
#include "rhi/qshader.h"
#include "rhi/qshaderbaker.h"

QShader newShaderFromCode(QShader::Stage stage, const char* code) {
	QShaderBaker baker;
	baker.setGeneratedShaderVariants({ QShader::StandardShader });
	baker.setGeneratedShaders({
		QShaderBaker::GeneratedShader{QShader::Source::SpirvShader,QShaderVersion(100)},
		QShaderBaker::GeneratedShader{QShader::Source::GlslShader,QShaderVersion(430)},
		QShaderBaker::GeneratedShader{QShader::Source::MslShader,QShaderVersion(12)},
		QShaderBaker::GeneratedShader{QShader::Source::HlslShader,QShaderVersion(60)},
		});
	baker.setSourceString(code, stage);
	QShader shader = baker.bake();

	if (!shader.isValid()) {
		QStringList codelist = QString(code).split('\n');
		for (int i = 0; i < codelist.size(); i++) {
			qWarning() << i + 1 << codelist[i].toLocal8Bit().data();
		}
		qWarning(baker.errorMessage().toLocal8Bit());
	}
	return shader;
}

static float VertexData[] = {
	//position(xy)		texture coord(uv)
	 1.0f,   1.0f,		1.0f,  1.0f,
	 1.0f,  -1.0f,		0.0f,  1.0f,
	-1.0f,  -1.0f,		0.0f,  0.0f,
	-1.0f,   1.0f,		1.0f,  0.0f,
};

static uint32_t IndexData[] = {
	0,1,2,
	2,3,0
};

struct UniformBlock {
	alignas(16) QVector3D row0;
	alignas(16) QVector3D row1;
	alignas(16) QVector3D row2;
};

QWindow3DPainter::QWindow3DPainter()
{
}

void QWindow3DPainter::setupNDCQuad(QQuadF quad)
{
	mNDCQuad = quad;
}

void QWindow3DPainter::setupTexture(QRhiTexture* texture)
{
	mTexture = texture;
}

void QWindow3DPainter::compile()
{
	mVertexBuffer.reset(mRhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(VertexData)));
	mVertexBuffer->create();
	mIndexBuffer.reset(mRhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::IndexBuffer, sizeof(IndexData)));
	mIndexBuffer->create();
	mUniformBuffer.reset(mRhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(UniformBlock)));
	mUniformBuffer->create();

	mPipeline.reset(mRhi->newGraphicsPipeline());
	QRhiGraphicsPipeline::TargetBlend blendState;
	blendState.dstColor = QRhiGraphicsPipeline::One;
	blendState.srcColor = QRhiGraphicsPipeline::One;
	blendState.dstAlpha = QRhiGraphicsPipeline::One;
	blendState.srcAlpha = QRhiGraphicsPipeline::One;
	blendState.enable = false;

	mPipeline->setTargetBlends({ blendState });
	mPipeline->setSampleCount(mSampleCount);
	mPipeline->setDepthTest(false);

	QString vsCode = R"(#version 450
		layout(location = 0) in vec2 inPosition;
		layout(location = 1) in vec2 inUV;
		layout(location = 0) out vec2 vUV;
		layout (binding = 0) uniform UniformBlock{
			mat3 mat;
		}UBO;

		out gl_PerVertex { vec4 gl_Position; };

		void main(){
			vec3 pos =  UBO.mat * vec3(inPosition,1);
			gl_Position = vec4(pos.xy,0,pos.z);
			vUV = inUV;
			%1
		}
	)";
	QShader vs = newShaderFromCode(QShader::VertexStage, vsCode.arg(mRhi->isYUpInNDC() ? "	vUV.y = 1 - vUV.y;" : "").toLocal8Bit());

	QShader fs = newShaderFromCode(QShader::FragmentStage, R"(#version 450
		layout (binding = 1) uniform sampler2D uSamplerColor;
		layout (location = 0) in vec2 vUV;
		layout (location = 0) out vec4 outFragColor;

		void main() {
			outFragColor = vec4(texture(uSamplerColor, vUV).rgba);
		}
	)");
	mPipeline->setShaderStages({
		{ QRhiShaderStage::Vertex, vs },
		{ QRhiShaderStage::Fragment, fs }
	});
	QRhiVertexInputLayout inputLayout;
	inputLayout.setBindings({
		{ 4 * sizeof(float) }
	});
	inputLayout.setAttributes({
		{ 0, 0, QRhiVertexInputAttribute::Float2, 0 },
		{ 0, 1, QRhiVertexInputAttribute::Float2, 2 * sizeof(float) }
	});
	mSampler.reset(mRhi->newSampler(QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None, QRhiSampler::Repeat, QRhiSampler::Repeat));
	mSampler->create();

	mBindings.reset(mRhi->newShaderResourceBindings());
	mBindings->setBindings({
		QRhiShaderResourceBinding::uniformBuffer(0,QRhiShaderResourceBinding::VertexStage,mUniformBuffer.get()),
		QRhiShaderResourceBinding::sampledTexture(1,QRhiShaderResourceBinding::FragmentStage,mTexture,mSampler.get())
	});
	mBindings->create();

	mPipeline->setVertexInputLayout(inputLayout);
	mPipeline->setShaderResourceBindings(mBindings.get());
	mPipeline->setRenderPassDescriptor(mRenderPassDesc);
	mPipeline->create();
	bNeedUpdate = true;
}

void QWindow3DPainter::updateResource(QRhiResourceUpdateBatch* batch)
{
	if (bNeedUpdate) {
		batch->uploadStaticBuffer(mIndexBuffer.get(), IndexData);
		batch->uploadStaticBuffer(mVertexBuffer.get(), VertexData);
		bNeedUpdate = false;
	}

	QQuadF src({
		{-1,-1},
		{-1,1},
		{1,1},
		{1,-1},

	});
	QMatrix3x3 mat = QQuadF::calcTranfrom(src, mNDCQuad);

	UniformBlock ubo;
	ubo.row0 = QVector3D(mat(0,0), mat( 1,0), mat( 2,0));
	ubo.row1 = QVector3D(mat(0,1), mat( 1,1), mat( 2,1));
	ubo.row2 = QVector3D(mat(0,2), mat( 1,2), mat( 2,2));

	auto a = QQuadF::transPoint(mat, src.topLeft);
	auto b = QQuadF::transPoint(mat, src.topRight);
	auto c = QQuadF::transPoint(mat, src.bottomRight);
	auto d = QQuadF::transPoint(mat, src.bottomLeft);

	batch->updateDynamicBuffer(mUniformBuffer.get(), 0, sizeof(UniformBlock), &ubo);
}

void QWindow3DPainter::paint(QRhiCommandBuffer* cmdBuffer, QRhiRenderTarget* renderTarget) {
	if (mTexture) {
		cmdBuffer->setGraphicsPipeline(mPipeline.get());
		cmdBuffer->setViewport(QRhiViewport(0, 0, renderTarget->pixelSize().width(), renderTarget->pixelSize().height()));
		cmdBuffer->setShaderResources(mBindings.get());
		const QRhiCommandBuffer::VertexInput vertexBindings(mVertexBuffer.get(), 0);
		cmdBuffer->setVertexInput(0, 1, &vertexBindings, mIndexBuffer.get(), 0, QRhiCommandBuffer::IndexUInt32);
		cmdBuffer->drawIndexed(6);
	}
}

