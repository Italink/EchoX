#include "QWidgetVFXManager.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include "Render/RenderGraph/Painter/TexturePainter.h"

QWidgetVFXManager& QWidgetVFXManager::Get()
{
	static QWidgetVFXManager Instance;
	return Instance;
}

class QWidgetVFX : public IWidgetVFX {
public:
	QImage mImage;
	QShader mFadeOutFS;
	QRhiBufferRef mUniformBuffer;
	QRhiSamplerRef mSampler;
	QRhiTextureRef mTexture;
	QRhiGraphicsPipelineRef mPipeline;
	QRhiShaderResourceBindingsRef mBindings;
public:
	QWidgetVFX() {
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

	QRect assessWidget(QWidget* widget) override {
		mImage = widget->grab().toImage().convertedTo(QImage::Format_RGBA8888);
		widget->close();
		return IWidgetVFX::assessWidget(widget);
	}
	float getPlayDurationSec() const  override {
		return 5;
	};
	void play(float timeSec, QRenderGraphBuilder& builder) override {
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
			auto batch = rhi->nextResourceUpdateBatch();
			if (!mImage.isNull()) {
				batch->uploadTexture(mTexture.get(), mImage);
				mImage = QImage();
			}
			float process = 1.0 - timeSec / getPlayDurationSec();
			qDebug() << process;
			batch->updateDynamicBuffer(mUniformBuffer.get(), 0, sizeof(float), &process);
			cmdBuffer->beginPass(builder.getMainRenderTarget(), QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f), { 1.0f, 0 }, batch);
			cmdBuffer->setGraphicsPipeline(mPipeline.get());
			cmdBuffer->setViewport(QRhiViewport(0, 0, builder.getMainRenderTarget()->pixelSize().width(), builder.getMainRenderTarget()->pixelSize().height()));
			cmdBuffer->setShaderResources(mBindings.get());
			cmdBuffer->draw(4);
			cmdBuffer->endPass();
		});
	}
};

void QWidgetVFXManager::playWidgetCloseVFX(QWidget* inWidget)
{
	QWidgetVFX* VFX = new QWidgetVFX;
	addVFX(inWidget,VFX);
}

QWidgetVFXManager::QWidgetVFXManager()
	: mRenderer(new QWidgetVFXRenderer)
{
	QPushButton* bt = new QPushButton("SSS");
	bt->setMinimumSize(400, 400);
	bt->show();
	connect(bt, &QPushButton::clicked, this, [bt, this]() {
		playWidgetCloseVFX(bt);
	});

	bt = new QPushButton("SSS");
	bt->setMinimumSize(400, 400);
	bt->show();
	connect(bt, &QPushButton::clicked, this, [bt, this]() {
		playWidgetCloseVFX(bt);
	});

	bt = new QPushButton("SSS");
	bt->setMinimumSize(400, 400);
	bt->show();
	connect(bt, &QPushButton::clicked, this, [bt, this]() {
		playWidgetCloseVFX(bt);
	});

	bt = new QPushButton("SSS");
	bt->setMinimumSize(400, 400);
	bt->show();
	connect(bt, &QPushButton::clicked, this, [bt, this]() {
		playWidgetCloseVFX(bt);
	});

	mViewport = QWidget::createWindowContainer(mRenderer->maybeWindow());
	mViewport->setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);
	mViewport->setAttribute(Qt::WA_TranslucentBackground);
	mViewport->setAttribute(Qt::WA_Mapped);
	mViewport->show();
	//connect(mRenderer, &QWidgetVFXRenderer::asEmptied, mViewport, &QWidget::hide);
}

void QWidgetVFXManager::addVFX(QWidget* inWidget, IWidgetVFX* inVFX)
{
	QRect playArea = inVFX->assessWidget(inWidget);
	inVFX->mCachedPlayArea = playArea;
	if (mViewport->isHidden()) {
		mViewport->resize(1, 1);
		mViewport->setParent(nullptr);
		mViewport->show();
	}
	mRenderer->maybeWindow()->requestUpdate();
	mViewport->setGeometry(playArea);
	mRenderer->addVFX(inVFX);
}

