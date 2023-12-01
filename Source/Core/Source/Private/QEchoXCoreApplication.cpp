#include "QEchoXCoreApplication.h"
#include "Window3D/QWindow3D.h"
#include "Window3D/QWindow3DEditor.h"

#include "Render/RHI/QRhiHelper.h"

QEchoXCoreApplication::QEchoXCoreApplication(int& argc, char** argv)
	: QApplication(argc, argv)
	, mRhi(QRhiHelper::create()) {
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