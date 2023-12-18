#include "QWidgetVFXManager.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include "Render/RenderGraph/Painter/TexturePainter.h"
#include "Render/RHI/QRhiTransparencyWindowContainter.h"
#include "tracy/Tracy.hpp"
#include "WidgetCloseVFX/QWdigetCloseVFX_Diffusion.h"
#include <QTimer>

QWidgetVFXManager& QWidgetVFXManager::Get()
{
	static QWidgetVFXManager Instance;
	return Instance;
}

void QWidgetVFXManager::playWidgetCloseVFX(QWidget* inWidget)
{
	QWidgetCloseVFX_Diffusion* VFX = new QWidgetCloseVFX_Diffusion;
	addVFX(inWidget,VFX);
	QObject::connect(VFX, &IWidgetVFX::asStarted, inWidget, &QWidget::close);

}

QWidgetVFXManager::QWidgetVFXManager()
	: mRhiParams(QRhiHelper::InitParams({ QRhi::Vulkan ,QRhi::Flag(),QRhiSwapChain::NoVSync | QRhiSwapChain::SurfaceHasNonPreMulAlpha }))
	, mRenderer(new QWidgetVFXRenderer(mRhiParams))
{
	QPushButton* bt = new QPushButton("A");
	bt->setWindowFlag(Qt::FramelessWindowHint);
	bt->move(500, 300);
	bt->setMinimumSize(400, 400);
	bt->show();
	connect(bt, &QPushButton::clicked, this, [bt, this]() {
		playWidgetCloseVFX(bt);
	});

	bt = new QPushButton("B");
	bt->setWindowFlag(Qt::FramelessWindowHint);
	bt->move(500, 800);
	bt->setMinimumSize(400, 400);
	bt->show();
	connect(bt, &QPushButton::clicked, this, [bt, this]() {
		playWidgetCloseVFX(bt);
	});

	bt = new QPushButton("C");
	bt->setWindowFlag(Qt::FramelessWindowHint);
	bt->move(1000, 300);
	bt->setMinimumSize(400, 400);
	bt->show();
	connect(bt, &QPushButton::clicked, this, [bt, this]() {
		playWidgetCloseVFX(bt);
	});

	bt = new QPushButton("D");
	bt->setWindowFlag(Qt::FramelessWindowHint);
	bt->move(1000, 800);
	bt->setMinimumSize(400, 400);
	bt->show();
	connect(bt, &QPushButton::clicked, this, [bt, this]() {
		playWidgetCloseVFX(bt);
	});

	QImage image(800, 800, QImage::Format_RGBA8888);
	QPainter painter(&image);
	painter.fillRect(QRect(0, 0, 600, 800), Qt::red);
	painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
	painter.fillRect(QRect(400, 0, 400, 800), Qt::blue);
	painter.end();
	image.save("aaaaaaaaaaaaaa.png");

	mViewport = QRhiTransparencyWindowContainter::create(mRenderer->maybeWindow(),mRhiParams.backend);
	mViewport->setAttribute(Qt::WA_TransparentForMouseEvents);
	mViewport->setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);
	mViewport->setAttribute(Qt::WA_TranslucentBackground);
	mViewport->setAttribute(Qt::WA_Mapped);
	mViewport->show();
	mViewport->setGeometry(QRect(0,0,1,1));
	connect(mRenderer, &QWidgetVFXRenderer::asEmptied, mViewport, &QWidget::hide);
}

void QWidgetVFXManager::addVFX(QWidget* inWidget, IWidgetVFX* inVFX)
{
	QRect playGemotry = mRenderer->addVFX(inWidget, inVFX);
	if (mViewport->isHidden()) {
		mViewport->show();
		mRenderer->resetTimer();
	}
	mViewport->setGeometry(playGemotry);
	
}

