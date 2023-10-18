#ifndef QEchoXApplication_h__
#define QEchoXApplication_h__

#include <QApplication>
#include <QSystemTrayIcon>
#include "EchoXEditorAPI.h"

class QRhi;
class QEchoXMainEditor;
class QEchoXMenu;

#undef qApp
#define qApp (static_cast<QEchoXApplication *>(QCoreApplication::instance()))

class ECHOXEDITOR_API QEchoXApplication : public QApplication {
public:
	QEchoXApplication(int& argc, char** argv);

	static void preInitialize();

	QRhi* getGlobalRhi() const;
	QEchoXMainEditor* getMainEditor() const;
	QSystemTrayIcon* getSystemTrayIcon() const;
protected:
	bool notify(QObject* o, QEvent* e) override;
	void onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
private:
	QSharedPointer<QRhi> mRhi;
	QEchoXMainEditor* mMainEditor = nullptr;
	QSystemTrayIcon* mSysIcon = nullptr;
	QEchoXMenu* mSysTrayMenu = nullptr;
};

#endif // QEchoXApplication_h__
