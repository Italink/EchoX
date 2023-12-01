#ifndef IEchoXWidgetItem_h__
#define IEchoXWidgetItem_h__

#include "Project/IEchoXItem.h"
#include <QWidget>
#include "Window3D/QWindow3D.h"

class ECHOXCORE_API IEchoXWidgetItem : public IEchoXItem {
public:
	IEchoXWidgetItem();
	virtual QWidget* widget() = 0;
	void initialize();
	void activate() override;
	void deactivate() override; 
private:
	QSharedPointer<QWindow3D> mWindow;
};

#endif // IEchoXWidgetItem_h__
