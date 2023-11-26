#ifndef IEchoXItem_h__
#define IEchoXItem_h__

#include <QObject>
#include "EchoXCoreAPI.h"

class ECHOXCORE_API IEchoXItem : public QObject {
	Q_OBJECT
public:
	virtual QSize desiredSize() = 0;
};

#endif // IEchoXItem_h__
