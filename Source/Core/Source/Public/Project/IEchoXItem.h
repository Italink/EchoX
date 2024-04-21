#ifndef IEchoXItem_h__
#define IEchoXItem_h__

#include <QObject>
#include "EchoXCoreAPI.h"

class ECHOXCORE_API IEchoXItem : public QObject {
	Q_OBJECT
public:
	void activateInternal();
	void deactivateInternal();

	bool isActivated() const;
	void setActivated(bool inActivated);
protected:
	virtual void activate() {};
	virtual void deactivate() {};
private:
	bool bActivated = false;
};

#endif // IEchoXItem_h__
