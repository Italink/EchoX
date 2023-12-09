#ifndef IEchoXWidgetItem_h__
#define IEchoXWidgetItem_h__

#include "Project/IEchoXItem.h"
#include <QWidget>
#include "Window3D/QWindow3D.h"

class ECHOXCORE_API IEchoXWidgetItem : public IEchoXItem {
	Q_OBJECT
	Q_PROPERTY(QQuadF Quad READ getQuad WRITE setQuad)
public:
	IEchoXWidgetItem();
	virtual QWidget* widget() = 0;
	virtual QSize desiredSize() = 0;

	void initialize();
	void activate() override;
	void deactivate() override; 

	QQuadF getQuad() const;
	void setQuad(QQuadF inQuad);
private:
	QSharedPointer<QWindow3D> mWindow;
};

#endif // IEchoXWidgetItem_h__
