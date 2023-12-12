#ifndef IEchoXComponent_h__
#define IEchoXComponent_h__

#include <QObject>
#include "EchoXCoreAPI.h"

class ECHOXCORE_API IEchoXComponent : public QObject {
	Q_OBJECT
public:
	virtual void activate() {};
	virtual void deactivate() {};
};

#endif // IEchoXComponent_h__
