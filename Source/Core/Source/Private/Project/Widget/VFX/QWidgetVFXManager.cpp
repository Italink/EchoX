#include "QWidgetVFXManager.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include "Render/RenderGraph/Painter/TexturePainter.h"
#include "Render/RHI/QRhiTransparencyWindowContainter.h"
#include "tracy/Tracy.hpp"
#include "WidgetCloseVFX/QWidgetCloseVFX_Diffusion.h"
#include <QTimer>
#include "QWidgetVFXSettings.h"

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
	QWidgetVFXSettings::Register();

	mViewport = QRhiTransparencyWindowContainter::create(mRenderer->maybeWindow(), mRhiParams.backend);
	mViewport->setAttribute(Qt::WA_TransparentForMouseEvents);
	mViewport->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
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

