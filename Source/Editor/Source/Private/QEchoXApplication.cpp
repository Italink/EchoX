#include "QEchoXApplication.h"
#include "QWindow3D.h"
#include "framelesshelperwidgets_global.h"
#include "framelesswidgetshelper.h"
#include "QEchoXTrayMenu.h"
#include "Render/RHI/QRhiHelper.h"
#include "QEchoXMainWindow.h"
#include "Settings/Audio/QAudioAnalyseView.h"
#include "Utils/Serialization.h"
#include "Audio/QSmtcManager.h"
#include "Settings/Audio/QSmtcView.h"
#include "Project/QProjectsManager.h"
#include "QEchoXProjectsPanel.h"
#include "QEchoXProjectsPage.h"
#include "DetailView/QDetailView.h"
#include "Settings/QSettingsManager.h"
#include "Settings/QEchoXStyleSettings.h"

QEchoXApplication::QEchoXApplication(int& argc, char** argv)
	: QApplication(argc, argv)
	, mRhi(QRhiHelper::create())
	, mMainWindow(new QEchoXMainWindow)
	, mSysIcon(new QSystemTrayIcon(this))
	, mSysTrayMenu(new QEchoXMenu(nullptr))
{
	Q_INIT_RESOURCE(EchoXEditor);

	mSysIcon->setToolTip("EchoX");
	mSysIcon->setIcon(QIcon(":/Resources/SystemTrayIcon.png"));
	mSysIcon->setContextMenu(mSysTrayMenu);
	mSysIcon->show();
	mMainWindow->setMinimumSize(800, 600);
	mMainWindow->show();
	setQuitOnLastWindowClosed(false);
	connect(mSysIcon, &QSystemTrayIcon::activated, this, &QEchoXApplication::onActivatedSysTrayIcon);

	Serialization::registerBuiltinType();
	QAudioAnalyseManager::Get().startup();
	QSmtcManager::Get().startup();
	QProjectsManager::Get().loadProjects();

	QEchoXProjectsPage* panel = new QEchoXProjectsPage;
	panel->show();
	//QAudioAnalyseView* view = new QAudioAnalyseView;
	//view->show();

	////QSmtcView* sview = new QSmtcView;
	////sview->show();
	//qRegisterMetaType<TestInlineGadget>();
	//qRegisterMetaType<TestInlineObject>();
	//TestObject to;
	//QDetailView* view = new QDetailView;
	//view->setObject(new TestObject);
	//view->show();
	//to.Pixmap = QPixmap(5,5);
	//QCborMap data  = Serialization::toCbor(&to);
	//qDebug() << data;
	//TestObject from;
	//Serialization::fromCbor(&from, data);
	//qDebug() << Serialization::toCbor(&from);
}

QEchoXApplication::~QEchoXApplication() {
	QAudioAnalyseManager::Get().shutdown();
	QSmtcManager::Get().shutdown();
}

void QEchoXApplication::preInitialize()
{
	qputenv("FRAMELESSHELPER_FORCE_HIDE_WINDOW_FRAME_BORDER", "1");
	//FramelessConfigEntry{ "FRAMELESSHELPER_USE_CROSS_PLATFORM_QT_IMPLEMENTATION", "Options/UseCrossPlatformQtImplementation" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_FORCE_HIDE_WINDOW_FRAME_BORDER", "Options/ForceHideWindowFrameBorder" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_FORCE_SHOW_WINDOW_FRAME_BORDER", "Options/ForceShowWindowFrameBorder" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_DISABLE_WINDOWS_SNAP_LAYOUT", "Options/DisableWindowsSnapLayout" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_WINDOW_USE_ROUND_CORNERS", "Options/WindowUseRoundCorners" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_CENTER_WINDOW_BEFORE_SHOW", "Options/CenterWindowBeforeShow" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_ENABLE_BLUR_BEHIND_WINDOW", "Options/EnableBlurBehindWindow" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_FORCE_NON_NATIVE_BACKGROUND_BLUR", "Options/ForceNonNativeBackgroundBlur" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_DISABLE_LAZY_INITIALIZATION_FOR_MICA_MATERIAL", "Options/DisableLazyInitializationForMicaMaterial" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_FORCE_NATIVE_BACKGROUND_BLUR", "Options/ForceNativeBackgroundBlur" },
	//	FramelessConfigEntry{ "FRAMELESSHELPER_WINDOW_USE_SQUARE_CORNERS", "Options/WindowUseSquareCorners" }
	FRAMELESSHELPER_NAMESPACE::FramelessHelper::Widgets::initialize();

}

QRhi* QEchoXApplication::getGlobalRhi() const
{
	return mRhi.get();
}

QEchoXMainWindow* QEchoXApplication::getMainEditor() const
{
	return mMainWindow;
}

QSystemTrayIcon* QEchoXApplication::getSystemTrayIcon() const
{
	return mSysIcon;
}

bool QEchoXApplication::notify(QObject* o, QEvent* e)
{
	if(QWindow3D::notify(o, e))
		return false;
	return QApplication::notify(o, e);
}

void QEchoXApplication::onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger: {
		if (!mMainWindow->isVisible()) {
			mMainWindow->show();
			mMainWindow->activateWindow();
		}
		else {
			mMainWindow->close();
		}
		break;
	}
	case QSystemTrayIcon::Context:
		break;
	default:
		break;
	}
}

