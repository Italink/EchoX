#ifndef QWindow3DEditor_h__
#define QWindow3DEditor_h__

#include <QWidget>
#include "QWindow3D.h"
#include "EchoXCoreAPI.h"

class QWindow3DEditorVertex : public QWidget{
	Q_OBJECT
private:
	QPointF mGlobalPos;
	QString mText;
	QColor mColor;
	QColor mHoverColor;
	bool bHovered = false;
public:
	explicit QWindow3DEditorVertex(QString inText,QColor inColor,QColor inHoverColor);
	QPointF getGlobalPos();
	void updateGeometry();
	void setGlobalPos(QPointF pos);
	void setId(const QString& value);
protected:
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event)override;
	virtual void paintEvent(QPaintEvent*) override;
	virtual void enterEvent(QEnterEvent*) override;
	virtual void leaveEvent(QEvent*) override;
Q_SIGNALS:
	void moved(QPointF);
};

class ECHOXCORE_API QWindow3DEditor : public QWidget
{
	Q_OBJECT
Q_SIGNALS:
	void asClicked();
private:
	QScopedPointer<QWindow3DEditorVertex> mVertex[4];
	QPoint mClickPos;
	QWindow3D* mHitWindow = nullptr;
	QWindow3D* mCurrentWindow = nullptr;
public:
	static QWindow3DEditor* Instance();
private:
	QWindow3DEditor();
	QQuadF getQuad();
	void setCurrentWindow(QWindow3D* inWindow);
protected:
	void notifyQuadChanged();
	virtual void showEvent(QShowEvent* event) override;
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void focusOutEvent(QFocusEvent* event) override;
};

#endif // QWindow3DEditor_h__
