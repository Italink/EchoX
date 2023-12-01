#include "QWindow3D.h"
#include "QWindow3DEffect.h"
#include "QWindow3DEditor.h"
#include <QApplication>
#include <private/qwidgetwindow_p.h>
#include <private/qcoreapplication_p.h>
#include <private/qwidgetrepaintmanager_p.h>
#include <QResizeEvent>

QList<QWindow3D*> QWindow3D::Instances;

void QWindow3D::showEditor()
{
	QWindow3DEditor* editor = new QWindow3DEditor;
	editor->show();
}

QWindow3D::QWindow3D()
	: mEffect(new QWindow3DEffect())
{
	setWindowFlag(Qt::FramelessWindowHint);
	Instances << this;
	//setAttribute(Qt::WA_TranslucentBackground);
}

QWindow3D::~QWindow3D()
{
	Instances.removeOne(this);
}

void QWindow3D::setupBody(QWidget* widget)
{
	mBody = widget;
	mBody->updateGeometry();
	qDebug() << mBody->geometry();
	mGlobalQuad = mBody->geometry();
	mBody->setParent(this);
	mBody->setGraphicsEffect(mEffect);
	mEffect->setupWidget(this, widget);
}

void QWindow3D::setGlobalQuad(QQuadF quad)
{
	mGlobalQuad = quad;
	QRect globalRect = QRect(mBody->mapToGlobal(QPoint(0, 0)), mBody->size());
	mGlobalQuadToRect = QQuadF::calcTranfrom(mGlobalQuad, globalRect);
	mGlobalRectToQuad = QQuadF::calcTranfrom(globalRect, mGlobalQuad);
	updateQuad();
	update();
}

QMatrix3x3 QWindow3D::getTransform()
{
	return mGlobalRectToQuad;
}

void QWindow3D::updateQuad()
{
	QRectF rect = mGlobalQuad.boundGemotry();
	setGeometry(rect.toRect());
}

QQuadF QWindow3D::getGlobalQuad()
{
	return mGlobalQuad;
}

QQuadF QWindow3D::getLocalQuad()
{
	return mGlobalQuad + QPointF(-geometry().topLeft());
}

QQuadF QWindow3D::getLoaclNDCQuad()
{
	QQuadF local = getLocalQuad();
	return local * QPointF(2.0f / width(), 2.0f / height()) + QPointF(-1.0f, -1.0f);
}

QPointF QWindow3D::mapGlobalPos(QPointF point)
{
	return QQuadF::transPoint(mGlobalQuadToRect, point);
}

bool QWindow3D::notify(QObject* o, QEvent* e)
{ 
	static QSet<QEvent*> visited;
	if (visited.contains(e))
		return false;
	if (e->type() == QEvent::MouseButtonPress
		|| e->type() == QEvent::MouseButtonRelease
		|| e->type() == QEvent::MouseMove
		) { 
		if (QString(o->metaObject()->className()) == "QWidgetWindow") {
			QWidgetWindow* qww = static_cast<QWidgetWindow*>(o);
			if (QWindow3D* q3d = qobject_cast<QWindow3D*>(qww->widget())) {
				QMouseEvent* mouseEvent = (QMouseEvent*)(e);
				QMouseEvent newMouseEvent(
					mouseEvent->type(),
					qww->mapFromGlobal(q3d->mapGlobalPos(mouseEvent->globalPosition())),
					mouseEvent->button(),
					mouseEvent->buttons(),
					mouseEvent->modifiers()
				);
				visited.insert(&newMouseEvent);
				QApplication::sendEvent(o, &newMouseEvent);
				visited.remove(&newMouseEvent);
				return true;
			}
		}
	}
	else if (e->type() == QEvent::ContextMenu) {
		if (QString(o->metaObject()->className()) == "QWidgetWindow") {
			QWidgetWindow* qww = static_cast<QWidgetWindow*>(o);
			if (QWindow3D* q3d = qobject_cast<QWindow3D*>(qww->widget())) {
				QContextMenuEvent* ctxMenuEvent = (QContextMenuEvent*)(e);
				QPoint globalPos = q3d->mapGlobalPos(ctxMenuEvent->globalPos()).toPoint();
				QContextMenuEvent newCtxMenuEvent(
					ctxMenuEvent->reason(),
					qww->mapFromGlobal(globalPos),
					globalPos,
					ctxMenuEvent->modifiers()
				);
				visited.insert(&newCtxMenuEvent);
				QApplication::sendEvent(o, &newCtxMenuEvent);
				visited.remove(&newCtxMenuEvent);
				return true;
			}
		}
	}
	else if (e->type() == QEvent::Wheel) {
		if (QString(o->metaObject()->className()) == "QWidgetWindow") {
			QWidgetWindow* qww = static_cast<QWidgetWindow*>(o);
			if (QWindow3D* q3d = qobject_cast<QWindow3D*>(qww->widget())) {
				QWheelEvent* wheelEvent = (QWheelEvent*)(e);
				QPointF globalPos = q3d->mapGlobalPos(wheelEvent->globalPosition());
				QWheelEvent newWheelEvent(
					qww->mapFromGlobal(globalPos),
					globalPos,
					wheelEvent->pixelDelta(),
					wheelEvent->angleDelta(),
					wheelEvent->buttons(),
					wheelEvent->modifiers(),
					wheelEvent->phase(),
					wheelEvent->inverted(),
					wheelEvent->source(),
					wheelEvent->pointingDevice()
				);
				newWheelEvent.setTimestamp(wheelEvent->timestamp());
				QCoreApplicationPrivate::setEventSpontaneous(&newWheelEvent, true);
				visited.insert(&newWheelEvent);
				QApplication::sendEvent(o, &newWheelEvent);
				visited.remove(&newWheelEvent);
				return true;
			}
		}
	}
	else if (e->type() == QEvent::UpdateRequest) {
		if (QWindow3D* q3d = qobject_cast<QWindow3D*>(o)) {
			QWidgetPrivate* w = QWidgetPrivate::get(q3d);
			w->scrollRect(q3d->rect(), 0, 0);
		}
	}
	//else if (e->type() != QEvent::Paint) {
	//	qDebug() << o << e->type();
	//}
	return false;
}

