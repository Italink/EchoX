#include "QWindow3DEditor.h"
#include "QWindow3D.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPolygon>
#include <QApplication>
#include <QWindow>
#include "private/qhighdpiscaling_p.h"

QWindow3DEditorVertex::QWindow3DEditorVertex(QString inText, QColor inColor, QColor inHoverColor) 
	: mText(inText)
	, mColor(inColor)
	, mHoverColor(inHoverColor)
{
	setFixedSize(20, 20);
}

QPointF QWindow3DEditorVertex::getGlobalPos()
{
	return mGlobalPos;
}

void QWindow3DEditorVertex::updateGeometry()
{
	QRect re = rect();
	re.moveCenter(this->pos() + mapFromGlobal(mGlobalPos).toPoint());
	setGeometry(re);
	update();
}

void QWindow3DEditorVertex::setGlobalPos(QPointF pos)
{
	mGlobalPos = pos;
}

void QWindow3DEditorVertex::setId(const QString& value)
{
	update();
}

void QWindow3DEditorVertex::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		setGlobalPos(event->globalPos());
		emit moved(event->globalPos());
	}
}

void QWindow3DEditorVertex::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(bHovered ? mHoverColor : mColor);
	painter.drawEllipse(rect());
	painter.setPen(Qt::black);
	painter.drawText(rect(), Qt::AlignCenter, mText);
}

void QWindow3DEditorVertex::enterEvent(QEnterEvent*)
{
	bHovered = true;
	update();
}

void QWindow3DEditorVertex::leaveEvent(QEvent*)
{
	bHovered = false;
	update();
}

QWindow3DEditor::QWindow3DEditor()
{
	QRect fullscreens;
	for (auto screen : qApp->screens()) {
		QRect localRect = screen->availableGeometry();
		localRect.setSize(localRect.size() * screen->devicePixelRatio());
		fullscreens |= localRect;
	}
	fullscreens.setSize(fullscreens.size() * this->devicePixelRatioF());
	this->setGeometry(fullscreens);

	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	this->setAttribute(Qt::WA_TranslucentBackground);

	//mVertex[0].reset(new QWindow3DEditorVertex("A", Qt::white, Qt::black));
	//mVertex[1].reset(new QWindow3DEditorVertex("B", Qt::white, Qt::black));
	//mVertex[2].reset(new QWindow3DEditorVertex("C", Qt::white, Qt::black));
	//mVertex[3].reset(new QWindow3DEditorVertex("D", Qt::white, Qt::black));
	//mVertex[0]->setParent(this);
	//mVertex[1]->setParent(this);
	//mVertex[2]->setParent(this);
	//mVertex[3]->setParent(this);
	
	//connect(mVertex[0].get(), &QWindow3DEditorVertex::moved, this, [this](QPointF) { notifyQuadChanged(); });
	//connect(mVertex[1].get(), &QWindow3DEditorVertex::moved, this, [this](QPointF) { notifyQuadChanged(); });
	//connect(mVertex[2].get(), &QWindow3DEditorVertex::moved, this, [this](QPointF) { notifyQuadChanged(); });
	//connect(mVertex[3].get(), &QWindow3DEditorVertex::moved, this, [this](QPointF) { notifyQuadChanged(); });
}

void QWindow3DEditor::setQuad(QQuadF inQuad)
{
	//mVertex[0]->setGlobalPos(inQuad.topLeft);
	//mVertex[1]->setGlobalPos(inQuad.topRight);
	//mVertex[2]->setGlobalPos(inQuad.bottomRight);
	//mVertex[3]->setGlobalPos(inQuad.bottomLeft);
	notifyQuadChanged();
}

QQuadF QWindow3DEditor::getQuad()
{
	return QQuadF();
}

void QWindow3DEditor::notifyQuadChanged()
{
	//mVertex[0]->updateGeometry();
	//mVertex[1]->updateGeometry();
	//mVertex[2]->updateGeometry();
	//mVertex[3]->updateGeometry();
	update(); 
}

void QWindow3DEditor::showEvent(QShowEvent* event)
{
	QRect fullscreens;
	for (auto screen : qApp->screens()) {
		QRect localRect = screen->availableGeometry();
		localRect.setSize(localRect.size() * screen->devicePixelRatio());
		fullscreens |= localRect;
	}
	this->setGeometry(fullscreens);
}

void QWindow3DEditor::paintEvent(QPaintEvent* event)
{
	//mVertex[0]->updateGeometry();
	//mVertex[1]->updateGeometry();
	//mVertex[2]->updateGeometry();
	//mVertex[3]->updateGeometry();

	QPainter painter(this);
	//painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::black);
	painter.setBrush(QColor(255, 0, 0));

	auto drawQuadF = [&painter,this](const QQuadF& quad) {
		painter.drawPolygon(QPolygonF({ mapFromGlobal(quad.topLeft),mapFromGlobal(quad.topRight),mapFromGlobal(quad.bottomRight),mapFromGlobal(quad.bottomLeft)}));
	};	

	for (auto instance : QWindow3D::Instances) {
		drawQuadF(instance->getGlobalQuad());
	}
}

void QWindow3DEditor::mousePressEvent(QMouseEvent* event)
{
	QRect fullscreens;
	for (auto screen : qApp->screens()) {
		QRect localRect = screen->availableGeometry();
		localRect.setSize(localRect.size() * screen->devicePixelRatio());
		fullscreens |= localRect;
	}
	fullscreens.setSize(fullscreens.size());
	this->setGeometry(fullscreens);

	mClickPos = event->pos();
	if (event->button() == Qt::RightButton) {
		close();
	}
	QWidget::mousePressEvent(event);
}

void QWindow3DEditor::mouseMoveEvent(QMouseEvent* event)
{
	qDebug() << event->pos();
	if (event->buttons() & Qt::LeftButton) {
		//for (int i = 0; i < 4; i++) {
		//	vertex[i].setPos(vertex[i].getPos() + event->pos() - mClickPos);
		//}
		//if (current != nullptr) {
		//	current->setVertex({ transPoint(vertex[0].getPos()),
		//						transPoint(vertex[1].getPos()),
		//						transPoint(vertex[2].getPos()),
		//						transPoint(vertex[3].getPos()) });
		//}
		update();
	}
	mClickPos = event->pos();
}

void QWindow3DEditor::mouseReleaseEvent(QMouseEvent* event)
{
	//if (event->button() == Qt::RightButton)
	//	close();
}

void QWindow3DEditor::focusOutEvent(QFocusEvent* event)
{
	//close();
}
