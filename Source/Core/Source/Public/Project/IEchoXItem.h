#ifndef IEchoXItem_h__
#define IEchoXItem_h__

#include <QObject>
#include "EchoXCoreAPI.h"

class ECHOXCORE_API IEchoXItem : public QObject {
	Q_OBJECT
public:
	virtual void activate() {};
	virtual void deactivate() {};
};

#endif // IEchoXItem_h__
