#ifndef QEchoXApplication_h__
#define QEchoXApplication_h__

#include "QEchoXCoreApplication.h"
#include "EchoXEditorAPI.h"

class QEchoXMainWindow;
class QEchoXMenu;

#undef qApp
#define qApp (static_cast<QEchoXApplication *>(QCoreApplication::instance()))

class ECHOXEDITOR_API QEchoXApplication : public QEchoXCoreApplication {
public:
	QEchoXApplication(int& argc, char** argv);
	~QEchoXApplication();
	static void preInitialize();

	QEchoXMainWindow* getMainEditor() const;
	QSystemTrayIcon* getSystemTrayIcon() const;
protected:
	void onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
private:
	QEchoXMainWindow* mMainWindow = nullptr;
	QSystemTrayIcon* mSysIcon = nullptr;
	QEchoXMenu* mSysTrayMenu = nullptr;
};

#endif // QEchoXApplication_h__
