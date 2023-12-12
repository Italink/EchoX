#ifndef QWindow3D_h__
#define QWindow3D_h__

#include <QWidget>
#include <QMatrix3x3>
#include "QQuadF.h"
#include "EchoXCoreAPI.h"

class QWindow3DEditor;
class QWindow3DEffect;

class ECHOXCORE_API QWindow3D : public QWidget {
	Q_OBJECT
	Q_PROPERTY(QQuadF Quad READ getGlobalQuad WRITE setGlobalQuad)
public:
	QWindow3D();
	~QWindow3D();
	void setupBody(QWidget* widget);
	void setGlobalQuad(QQuadF quad);
	QMatrix3x3 getTransform();
	void updateQuad();
	QQuadF getGlobalQuad();
	QQuadF getLocalQuad();
	QQuadF getLoaclNDCQuad();
	QPointF mapGlobalPos(QPointF point);
	static bool notify(QObject* o, QEvent* e);
	static QList<QWindow3D*> Instances;
protected:
	void showEvent(QShowEvent* e) override;
private:
	QWidget* mBody = nullptr;
	QQuadF mGlobalQuad;
	QMatrix3x3 mGlobalRectToQuad;
	QMatrix3x3 mGlobalQuadToRect;
	QWindow3DEffect* mEffect = nullptr;
};

#endif // QWindow3D_h__
