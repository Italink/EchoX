#ifndef IEchoXVisualItem_h__
#define IEchoXVisualItem_h__

#include "IEchoXItem.h"
#include <QPointF>

class ECHOXCORE_API IEchoXVisualItem : public IEchoXItem {
public:
	virtual bool onItemPlacing(QPointF scenePosition);
	virtual bool isVisible();
	void setVisibility(bool bVisible);
private:
	bool bVisibility = true;
};

#endif // IEchoXVisualItem_h__
