#include "QEchoXApplication.h"
#include "QEchoXTrayMenu.h"
#include "Render/RHI/QRhiHelper.h"
#include "Settings/Audio/QAudioAnalyseView.h"
#include "Utils/Serialization.h"
#include "Audio/QSmtcManager.h"
#include "Settings/Audio/QSmtcView.h"
#include "Project/QProjectsManager.h"
#include "QEchoXProjectsPanel.h"
#include "QEchoXProjectsPage.h"
#include "Settings/QSettingsManager.h"
#include "Settings/QEchoXStyleSettings.h"
#include "Plugin/QEnginePluginManager.h"
#include "Project/Widget/QEchoXWidgetComponent_Button.h"
#include "QAudioAnalyseSettings.h"
#include "QSmtcSettings.h"
#include "Project/Widget/VFX/QWidgetVFXManager.h"
#include <QWKQuick/qwkquickglobal.h>
#include "QEchoXController.h"
#include "DetailView/QQuickDetailsViewMananger.h"
#include "DetailView/QQmlHelper.h"
#include "QEchoXPluginsSettings.h"

QEchoXApplication::QEchoXApplication(int& argc, char** argv)
	: QEchoXCoreApplication(argc, argv)
	, mQmlEngine(new QQmlApplicationEngine(this))
{
	Q_INIT_RESOURCE(EchoXEditor);

	Serialization::registerBuiltinType();
	QAudioAnalyseManager::Get().startup();
	//QSmtcManager::Get().startup();
	QProjectsManager::Get().loadProjects();
	QWidgetVFXManager::Get();

	QEchoXPluginsSettings::Register();
	QEchoXStyleSettings::Register();
	QAudioAnalyseSettings::Register();
	QSmtcSettings::Register();

	QProjectsManager::Get().registerItemType(&QEchoXWidgetComponent_Button::staticMetaObject);

	QEnginePluginManager::Get().loadPlugins();
	for (auto& pluginHandler : QEnginePluginManager::Get().getPluginHandlers()) {
		pluginHandler.startup();
	}

	mSysIcon = (new QSystemTrayIcon(this));
	mSysTrayMenu = (new QEchoXMenu(nullptr));
	mSysIcon->setToolTip("EchoX");
	mSysIcon->setIcon(QIcon(":/Resources/SystemTrayIcon.png"));
	mSysIcon->setContextMenu(mSysTrayMenu);
	mSysIcon->show();

	QWK::registerTypes(mQmlEngine);
	QQuickDetailsViewManager::Get()->registerQml();
	setQuitOnLastWindowClosed(false);
	connect(mSysIcon, &QSystemTrayIcon::activated, this, &QEchoXApplication::onActivatedSysTrayIcon);
	const QUrl url(QStringLiteral("qrc:/Resources/Qml/MainWindow.qml"));
	QObject::connect(mQmlEngine, &QQmlApplicationEngine::objectCreated,
		this, [url](QObject* obj, const QUrl& objUrl) {
			if (!obj && url == objUrl)
				QCoreApplication::exit(-1);
		}, Qt::QueuedConnection);
	mQmlEngine->rootContext()->setContextProperty("helper", QQmlHelper::Get());
	mQmlEngine->rootContext()->setContextProperty("EchoXStyle", QEchoXStyleSettings::Get());
	mQmlEngine->rootContext()->setContextProperty("Controller", QEchoXController::Get());
	mQmlEngine->load(url);
}

QEchoXApplication::~QEchoXApplication() {
	QEchoXStyleSettings::Unregister();

	for (auto& pluginHandler : QEnginePluginManager::Get().getPluginHandlers()) {
		pluginHandler.shutdown();
	}

	QAudioAnalyseManager::Get().shutdown();
	//QSmtcManager::Get().shutdown();
}

void QEchoXApplication::preInitialize()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
#else
	qputenv("QT_QUICK_CONTROLS_STYLE", "Default");
#endif
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	//QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
}

QEchoXMainWindow* QEchoXApplication::getMainEditor() const
{
	return mMainWindow;
}

QSystemTrayIcon* QEchoXApplication::getSystemTrayIcon() const
{
	return mSysIcon;
} 

void QEchoXApplication::onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
	//switch (reason) {
	//case QSystemTrayIcon::Trigger: {
	//	if (!mMainWindow->isVisible()) {
	//		mMainWindow->show();
	//		mMainWindow->activateWindow();
	//	}
	//	else {
	//		mMainWindow->close();
	//	}
	//	break;
	//}
	//case QSystemTrayIcon::Context:
	//	break;
	//default:
	//	break;
	//}
}

