#ifndef QEchoXApplication_h__
#define QEchoXApplication_h__

#include <QApplication>
#include <QSystemTrayIcon>
#include "EchoXEditorAPI.h"
#include "qvectornd.h"

class QRhi;
class QEchoXMainWindow;
class QEchoXMenu;

#undef qApp
#define qApp (static_cast<QEchoXApplication *>(QCoreApplication::instance()))

class ECHOXEDITOR_API QEchoXApplication : public QApplication {
public:
	QEchoXApplication(int& argc, char** argv);
	~QEchoXApplication();
	static void preInitialize();

	QRhi* getGlobalRhi() const;
	QEchoXMainWindow* getMainEditor() const;
	QSystemTrayIcon* getSystemTrayIcon() const;
protected:
	bool notify(QObject* o, QEvent* e) override;
	void onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
private:
	QSharedPointer<QRhi> mRhi;
	QEchoXMainWindow* mMainWindow = nullptr;
	QSystemTrayIcon* mSysIcon = nullptr;
	QEchoXMenu* mSysTrayMenu = nullptr;
};

#endif // QEchoXApplication_h__
