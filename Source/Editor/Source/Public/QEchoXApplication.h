#ifndef QEchoXApplication_h__
#define QEchoXApplication_h__

#include "QEchoXCoreApplication.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "EchoXEditorAPI.h"
#include "QEchoXItemControlLayerRenderer.h"

class QEchoXMainWindow;
class QEchoXMenu;

#undef qApp
#define qApp (static_cast<QEchoXApplication *>(QCoreApplication::instance()))

class ECHOXEDITOR_API QEchoXApplication : public QEchoXCoreApplication {
public:
	QEchoXApplication(int& argc, char** argv);
	~QEchoXApplication();
	static void preInitialize();

	QSystemTrayIcon* getSystemTrayIcon() const;
protected:
	void onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
	void onScreenSizeChanged();
private:
	QSystemTrayIcon* mSysIcon = nullptr;
	QEchoXMenu* mSysTrayMenu = nullptr;
	QQmlApplicationEngine* mQmlEngine;

	QRhiHelper::InitParams mRhiParams;
	QEchoXItemControlLayerRenderer* mItemControlLayerRenderer = nullptr;
	QWidget* mItemControlLayerViewport = nullptr;
};

#endif // QEchoXApplication_h__
