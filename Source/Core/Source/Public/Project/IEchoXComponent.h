#ifndef IEchoXComponent_h__
#define IEchoXComponent_h__

#include <QObject>
#include "EchoXCoreAPI.h"

class ECHOXCORE_API IEchoXComponent : public QObject {
	Q_OBJECT
public:
	bool isActivated() const;
	void activateInternal();
	void deactivateInternal();

	void setActivated(bool inActivated);
	bool getActivated() const;
protected:
	virtual void activate() {};
	virtual void deactivate() {};
private:
	bool bActivated = false;
};

#endif // IEchoXComponent_h__
