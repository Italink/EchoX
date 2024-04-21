#include "IEchoX3DItem.h"
#include "Render/RHI/QRhiTransparencyWindowContainter.h"

IEchoX3DItem::IEchoX3DItem()
{

}

void IEchoX3DItem::activate()
{
	IEchoXVisualItem::activate();
	if (!mRenderer) {
		mRenderer = createRenderer();
		mViewport = QRhiTransparencyWindowContainter::create(mRenderer->maybeWindow(), QRhi::Vulkan);
		mViewport->setAttribute(Qt::WA_TransparentForMouseEvents);
		mViewport->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
		mViewport->setAttribute(Qt::WA_TranslucentBackground);
		mViewport->setAttribute(Qt::WA_Mapped);
		mViewport->resize(100, 100);
	}
	mViewport->show();
}
  
void IEchoX3DItem::deactivate()
{
	IEchoXVisualItem::deactivate();
	mViewport->close();
}

