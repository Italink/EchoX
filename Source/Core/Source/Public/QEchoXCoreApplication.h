#ifndef QEchoXCoreApplication_h__
#define QEchoXCoreApplication_h__

#include <QApplication>
#include <QSystemTrayIcon>
#include "EchoXCoreAPI.h"

class QRhi;

class ECHOXCORE_API QEchoXCoreApplication : public QApplication {
public:
	QEchoXCoreApplication(int& argc, char** argv);
	~QEchoXCoreApplication();
	QRhi* getGlobalRhi() const;
protected:
	bool notify(QObject* o, QEvent* e) override;
protected:
	QSharedPointer<QRhi> mRhi;
};

#endif // QEchoXCoreApplication_h__
