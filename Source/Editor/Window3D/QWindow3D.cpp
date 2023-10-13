#include "QWindow3D.h"
#include "QWindow3DEffect.h"
#include "QWindow3DEditor.h"
#include <QApplication>
#include <private/qwidgetwindow_p.h>
#include <private/qwidgetrepaintmanager_p.h>
#include <QResizeEvent>

QWindow3D::QWindow3D()
	: mEffect(new QWindow3DEffect())
{
	setWindowFlag(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
}

void QWindow3D::setupBody(QWidget* widget)
{
	mBody = widget;
	mBody->updateGeometry();
	mGlobalQuad = mBody->geometry();
	mBody->setParent(this);
	mBody->setGraphicsEffect(mEffect);
	mEffect->setupWidget(this, widget);
}

void QWindow3D::showEvent(QShowEvent* event)
{
	if (mEditor) {
		mEditor->setQuad(getGlobalQuad());
	}
}

void QWindow3D::resizeEvent(QResizeEvent* event)
{
	if (mEditor) {
		mEditor->setGeometry(geometry().adjusted(-10, -10, 10, 10));
	}
}

QWindow3D* QWindow3D::create(QWidget* widget)
{
	QWindow3D* containter = new QWindow3D;
	containter->setupBody(widget);
	return containter;
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

void QWindow3D::showEditor()
{
	if (!mEditor) {
		mEditor = new QWindow3DEditor;
		mEditor->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
		mEditor->setAttribute(Qt::WA_TranslucentBackground);
		mEditor->show();
		connect(mEditor, &QWindow3DEditor::quadChanged, this, &QWindow3D::setGlobalQuad);
	}
	if (this->isVisible()) {
		mEditor->show();
	}
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

bool QWindow3D::notify(QApplication* app, QObject* o, QEvent* e)
{
	if (e->type() == QEvent::MouseButtonPress
		|| e->type() == QEvent::MouseButtonRelease
		|| e->type() == QEvent::MouseMove
		) {
		if (QString(o->metaObject()->className()) == "QWidgetWindow") {
			QWidgetWindow* qww = static_cast<QWidgetWindow*>(o);
			if (QWindow3D* q3d = qobject_cast<QWindow3D*>(qww->widget())) {
				QMouseEvent* mouseEvent = (QMouseEvent*)(e);
				QMouseEvent* newMouseEvent = new QMouseEvent(
					mouseEvent->type(),
					qww->mapFromGlobal(q3d->mapGlobalPos(mouseEvent->globalPosition())),
					mouseEvent->button(),
					mouseEvent->buttons(),
					mouseEvent->modifiers()
				);
				return app->notify(o, newMouseEvent);
			}
			auto c = qww->widget();
		}
	}
	else if (e->type() == QEvent::Wheel) {
		if (QString(o->metaObject()->className()) == "QWidgetWindow") {
			QWidgetWindow* qww = static_cast<QWidgetWindow*>(o);
			if (QWindow3D* q3d = qobject_cast<QWindow3D*>(qww->widget())) {
				QWheelEvent* wheelEvent = (QWheelEvent*)(e);
				QPointF globalPos = q3d->mapGlobalPos(wheelEvent->globalPosition());
				QWheelEvent* newWheelEvent = new QWheelEvent(
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
				return app->notify(o, newWheelEvent);
			}
			auto c = qww->widget();
		}
	}
	if (e->type() == QEvent::UpdateRequest) {
		if (QWindow3D* q3d = qobject_cast<QWindow3D*>(o)) {
			QWidgetPrivate* w = QWidgetPrivate::get(q3d);
			w->scrollRect(q3d->rect(), 0, 0);
		}
	}
	return app->notify(o, e);
}
