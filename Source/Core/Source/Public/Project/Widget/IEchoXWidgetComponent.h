#ifndef IEchoXWidgetComponent_h__
#define IEchoXWidgetComponent_h__

#include "Project/IEchoXComponent.h"
#include <QWidget>
#include "Project/Widget/3D/QWindow3D.h"

class ECHOXCORE_API IEchoXWidgetComponent: public IEchoXComponent {
	Q_OBJECT
	Q_PROPERTY(QQuadF Quad READ getQuad WRITE setQuad)
public:
	IEchoXWidgetComponent();

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

#endif // IEchoXWidgetComponent_h__
