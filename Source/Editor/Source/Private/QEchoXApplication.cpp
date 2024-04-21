#include "QEchoXApplication.h"
#include "QEchoXTrayMenu.h"
#include "Render/RHI/QRhiHelper.h"
#include "Settings/Audio/QAudioAnalyseView.h"
#include "Utils/Serialization.h"
#include "Audio/QSmtcManager.h"
#include "Settings/Audio/QSmtcView.h"
#include "Project/QEchoXProjectsManager.h"
#include "Settings/QSettingsManager.h"
#include "Settings/QEchoXStyleSettings.h"
#include "Plugin/QEnginePluginManager.h"
#include "Project/Widget/QEchoXWidgetItem_Button.h"
#include "QAudioAnalyseSettings.h"
#include "QSmtcSettings.h"
#include "Project/Widget/VFX/QWidgetVFXManager.h"
#include <QWKQuick/qwkquickglobal.h>
#include "QEchoXController.h"
#include "DetailView/QQuickDetailsViewMananger.h"
#include "DetailView/QQmlHelper.h"
#include "QEchoXPluginsSettings.h"
#include "Render/RHI/QRhiTransparencyWindowContainter.h"
#include <QScreen>

QEchoXApplication::QEchoXApplication(int& argc, char** argv)
	: QEchoXCoreApplication(argc, argv)
	, mQmlEngine(new QQmlApplicationEngine(this))
{
	Q_INIT_RESOURCE(EchoXEditor);

	Serialization::registerBuiltinType();
	QAudioAnalyseManager::Get().startup();
	//QSmtcManager::Get().startup();
	QEchoXProjectsManager::Get().loadProjects();
	QWidgetVFXManager::Get();

	QEchoXPluginsSettings::Register();
	QEchoXStyleSettings::Register();
	QAudioAnalyseSettings::Register();
	QSmtcSettings::Register();

	QEchoXProjectsManager::Get().registerItemType(&QEchoXWidgetItem_Button::staticMetaObject);

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

	mRhiParams.backend = QRhi::D3D12;

	mItemControlLayerRenderer = new QEchoXItemControlLayerRenderer(mRhiParams);
	mItemControlLayerRenderer->setParent(this);

	mItemControlLayerViewport = QRhiTransparencyWindowContainter::create(mItemControlLayerRenderer->maybeWindow(), mRhiParams.backend);
	mItemControlLayerViewport->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	mItemControlLayerViewport->setAttribute(Qt::WA_TranslucentBackground);
	mItemControlLayerViewport->setAttribute(Qt::WA_Mapped);
	mItemControlLayerViewport->setGeometry(QRect(0, 0, 1920, 1080));
	mItemControlLayerViewport->show();

	onScreenSizeChanged();
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
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
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

void QEchoXApplication::onScreenSizeChanged()
{
	QList<QScreen*> screens = this->screens();
	QRect allGeomtry;
	for (auto screen : screens) {
		allGeomtry |= screen->availableGeometry();
	}
	mItemControlLayerViewport->setGeometry(allGeomtry);
}

