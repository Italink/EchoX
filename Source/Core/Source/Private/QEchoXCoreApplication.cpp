#include "QEchoXCoreApplication.h"
#include "Project/Widget/3D/QWindow3D.h"
#include "Project/Widget/3D/QWindow3DEditor.h"

#include "Render/RHI/QRhiHelper.h"

QEchoXCoreApplication::QEchoXCoreApplication(int& argc, char** argv)
	: QApplication(argc, argv)
	, mRhi(QRhiHelper::create()) {

	QSurfaceFormat fmt;
	fmt.setSamples(1);
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
	if(QWindow3D::notify(o, e))
		return false;
	return QApplication::notify(o, e);
}