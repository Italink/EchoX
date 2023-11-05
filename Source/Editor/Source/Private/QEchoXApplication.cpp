#include "QEchoXApplication.h"
#include "QWindow3D.h"
#include "framelesshelperwidgets_global.h"
#include "framelesswidgetshelper.h"
#include "QEchoXTrayMenu.h"
#include "Render/RHI/QRhiHelper.h"
#include "QEchoXMainEditor.h"
#include "Settings/AudioAnalyse/QAudioAnalyseView.h"
#include "Utils/Serialization.h"
#include "Audio/QSmtcManager.h"
#include "Settings/Smtc/QSmtcView.h"

QEchoXApplication::QEchoXApplication(int& argc, char** argv)
	: QApplication(argc, argv)
	, mRhi(QRhiHelper::create())
	, mMainEditor(new QEchoXMainEditor)
	, mSysIcon(new QSystemTrayIcon(this))
	, mSysTrayMenu(new QEchoXMenu(nullptr))
{
	Q_INIT_RESOURCE(EchoXEditor);

	mSysIcon->setToolTip("EchoX");
	mSysIcon->setIcon(QIcon(":/Resources/SystemTrayIcon.png"));
	mSysIcon->setContextMenu(mSysTrayMenu);
	mSysIcon->show();
	mMainEditor->setMinimumSize(800, 600);
	mMainEditor->show();
	setQuitOnLastWindowClosed(false);
	connect(mSysIcon, &QSystemTrayIcon::activated, this, &QEchoXApplication::onActivatedSysTrayIcon);

	QAudioAnalyseManager::Get().startup();
	QSmtcManager::Get().startup();

	QAudioAnalyseView* view = new QAudioAnalyseView;
	view->show();

	QSmtcView* sview = new QSmtcView;
	sview->show();

	TestObject t;
	qDebug()<< Serialization::toJson(&t);

}

QEchoXApplication::~QEchoXApplication() {
	QAudioAnalyseManager::Get().shutdown();
	QSmtcManager::Get().shutdown();
}

void QEchoXApplication::preInitialize()
{
	FRAMELESSHELPER_NAMESPACE::FramelessHelper::Widgets::initialize();
}

QRhi* QEchoXApplication::getGlobalRhi() const
{
	return mRhi.get();
}

QEchoXMainEditor* QEchoXApplication::getMainEditor() const
{
	return mMainEditor;
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
		if (!mMainEditor->isVisible()) {
			mMainEditor->show();
			mMainEditor->activateWindow();
		}
		else {
			mMainEditor->close();
		}
		break;
	}
	case QSystemTrayIcon::Context:
		break;
	default:
		break;
	}
}

