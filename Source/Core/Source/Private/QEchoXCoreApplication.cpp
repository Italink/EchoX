#include "QEchoXCoreApplication.h"
#include "Project/Widget/3D/QWindow3D.h"
#include "Render/RHI/QRhiHelper.h"
#include "tracy/Tracy.hpp"

QEchoXCoreApplication::QEchoXCoreApplication(int& argc, char** argv)
	: QApplication(argc, argv)
	, mRhi(QRhiHelper::create()) {

	QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
	fmt.setSamples(1);
	fmt.setRedBufferSize(8);
	fmt.setGreenBufferSize(8);
	fmt.setBlueBufferSize(8);
	//fmt.setStereo(true);
	fmt.setAlphaBufferSize(8);
	QSurfaceFormat::setDefaultFormat(fmt);
}

QEchoXCoreApplication::~QEchoXCoreApplication() {
}


QRhi* QEchoXCoreApplication::getGlobalRhi() const
{
	return mRhi.get();
}

bool QEchoXCoreApplication::notify(QObject* o, QEvent* e)
{
	ZoneScopedN("Notify");
	if(QWindow3D::notify(o, e))
		return false;
	return QApplication::notify(o, e);
}