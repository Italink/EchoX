#ifndef IEchoXSettings_h__
#define IEchoXSettings_h__

#include <QObject>
#include "EchoXCoreAPI.h"

class ECHOXCORE_API IEchoXSettings: public QObject {
	Q_OBJECT
public:
	virtual QString path() = 0;
	virtual QWidget* createWidget() = 0;
};

#endif // IEchoXSettings_h__
