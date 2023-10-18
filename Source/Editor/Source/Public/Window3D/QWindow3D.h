#ifndef QWindow3D_h__
#define QWindow3D_h__

#include <QWidget>
#include <QMatrix3x3>
#include "QQuadF.h"
#include "EchoXEditorAPI.h"

class QWindow3DEditor;
class QWindow3DEffect;

class ECHOXEDITOR_API QWindow3D : public QWidget {
	Q_OBJECT
public:
	static QWindow3D* create(QWidget* widget);
	void setGlobalQuad(QQuadF quad);
	QMatrix3x3 getTransform();
	void updateQuad();
	void showEditor();
	QQuadF getGlobalQuad();
	QQuadF getLocalQuad();
	QQuadF getLoaclNDCQuad();
	QPointF mapGlobalPos(QPointF point);
	static bool notify(QObject* o, QEvent* e);
private:
	QWindow3D();
	void setupBody(QWidget* widget);
	void showEvent(QShowEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	QWidget* mBody = nullptr;
	QWindow3DEditor* mEditor = nullptr;
	QQuadF mGlobalQuad;
	QMatrix3x3 mGlobalRectToQuad;
	QMatrix3x3 mGlobalQuadToRect;
	QWindow3DEffect* mEffect = nullptr;
};


#endif // QWindow3D_h__
