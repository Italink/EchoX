#include "QWidgetVFXManager.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>

QWidgetVFXManager& QWidgetVFXManager::Get()
{
	static QWidgetVFXManager Instance;
	return Instance;
}

class QWidgetVFX : public IWidgetVFX {
	float getPlayDurationSec() const  override { 
		return 5;
	};

	void play(float progress, QRenderGraphBuilder& graphBuilder) override{
		qDebug() << progress;
		graphBuilder.addPass([&graphBuilder](QRhiCommandBuffer* cmdBuffer) {
			cmdBuffer->beginPass(graphBuilder.getMainRenderTarget(), QColor::fromRgbF(1.0f, 0.0f, 0.0f, 1.0f), { 1.0f, 0 });
			cmdBuffer->endPass();
		});
	}
};


void QWidgetVFXManager::playWidgetCloseVFX(QWidget* inWidget)
{
	QWidgetVFX* VFX = new QWidgetVFX;
	VFX->setupWidget(inWidget);
	addVFX(VFX);
}

QWidgetVFXManager::QWidgetVFXManager()
	: mRenderer(new QWidgetVFXRenderer)
{
	QPushButton* bt = new QPushButton("Bt");
	bt->setMinimumSize(400, 400);
	bt->show();
	mViewport = QWidget::createWindowContainer(mRenderer->maybeWindow());
	mViewport->setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);

	mViewport->setAttribute(Qt::WA_TranslucentBackground);
	connect(bt, &QPushButton::clicked, this, [bt,this]() {
		playWidgetCloseVFX(bt);
	});
}

void QWidgetVFXManager::addVFX(IWidgetVFX* inVFX)
{
	mViewport->show();
	mViewport->setGeometry(inVFX->assessPlayArea());
	mRenderer->addVFX(inVFX);
}

