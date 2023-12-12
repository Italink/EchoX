#include "QWindow3DEditor.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPolygon>
#include <QApplication>
#include <QWindow>
#include "private/qhighdpiscaling_p.h"
#include "DetailView/QPropertyHandle.h"

QWindow3DEditorVertex::QWindow3DEditorVertex(QString inText, QColor inColor, QColor inHoverColor) 
	: mText(inText)
	, mColor(inColor)
	, mHoverColor(inHoverColor)
{
	setFixedSize(20, 20);
	this->setCursor(Qt::CursorShape::ClosedHandCursor);
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

void QWindow3DEditorVertex::mousePressEvent(QMouseEvent* event)
{
	event->accept();
}

void QWindow3DEditorVertex::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		setGlobalPos(event->globalPosition());
		emit moved(event->globalPosition());
		event->accept();
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

QWindow3DEditor* QWindow3DEditor::Instance()
{
	static QWindow3DEditor Inst;
	return &Inst;
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

	mVertex[0].reset(new QWindow3DEditorVertex("A", Qt::white, Qt::black));
	mVertex[1].reset(new QWindow3DEditorVertex("B", Qt::white, Qt::black));
	mVertex[2].reset(new QWindow3DEditorVertex("C", Qt::white, Qt::black));
	mVertex[3].reset(new QWindow3DEditorVertex("D", Qt::white, Qt::black));

	mVertex[0]->setParent(this);
	mVertex[1]->setParent(this);
	mVertex[2]->setParent(this);
	mVertex[3]->setParent(this);
	
	connect(mVertex[0].get(), &QWindow3DEditorVertex::moved, this, [this](QPointF) { notifyQuadChanged(); });
	connect(mVertex[1].get(), &QWindow3DEditorVertex::moved, this, [this](QPointF) { notifyQuadChanged(); });
	connect(mVertex[2].get(), &QWindow3DEditorVertex::moved, this, [this](QPointF) { notifyQuadChanged(); });
	connect(mVertex[3].get(), &QWindow3DEditorVertex::moved, this, [this](QPointF) { notifyQuadChanged(); });
}

QQuadF QWindow3DEditor::getQuad()
{
	return QQuadF(
		mVertex[0]->getGlobalPos(),
		mVertex[1]->getGlobalPos(),
		mVertex[2]->getGlobalPos(),
		mVertex[3]->getGlobalPos()
	);
}

void QWindow3DEditor::setCurrentWindow(QWindow3D* inWindow)
{
	if (inWindow != mCurrentWindow) {
		if (inWindow) {
			QQuadF quad = inWindow->getGlobalQuad();
			mVertex[0]->setVisible(true);
			mVertex[1]->setVisible(true);
			mVertex[2]->setVisible(true);
			mVertex[3]->setVisible(true);
			mVertex[0]->setGlobalPos(quad.topLeft);
			mVertex[1]->setGlobalPos(quad.topRight);
			mVertex[2]->setGlobalPos(quad.bottomRight);
			mVertex[3]->setGlobalPos(quad.bottomLeft);
			mVertex[0]->updateGeometry();
			mVertex[1]->updateGeometry();
			mVertex[2]->updateGeometry();
			mVertex[3]->updateGeometry();
		}
		else {
			mVertex[0]->setVisible(false);
			mVertex[1]->setVisible(false);
			mVertex[2]->setVisible(false);
			mVertex[3]->setVisible(false);
		}
		mCurrentWindow = inWindow;
	}
}

void QWindow3DEditor::notifyQuadChanged()
{
	mVertex[0]->updateGeometry();
	mVertex[1]->updateGeometry();
	mVertex[2]->updateGeometry();
	mVertex[3]->updateGeometry();
	if (mCurrentWindow) {
		mCurrentWindow->setGlobalQuad(getQuad());
	}
	update(); 
	//if (QPropertyHandle* transform = QPropertyHandle::FindOrCreate(comp, "Transform"))
	//	transform->setValue(compModelMatrix, "Move");
	//if (QPropertyHandle* position = QPropertyHandle::Find(comp, "Transform.Position"))
	//	position->refreshBinder();
	//if (QPropertyHandle* rotation = QPropertyHandle::Find(comp, "Transform.Rotation"))
	//	rotation->refreshBinder();
	//if (QPropertyHandle* scale = QPropertyHandle::Find(comp, "Transform.Scale"))
	//	scale->refreshBinder();
	Q_EMIT asClicked();
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
	this->setMouseTracking(true);
}

void QWindow3DEditor::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(0, 0, 0, 20));
	//painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::red);
	painter.setBrush(Qt::NoBrush);

	auto drawQuadF = [&painter,this](const QQuadF& quad) {
		painter.drawPolygon(QPolygonF({ mapFromGlobal(quad.topLeft),mapFromGlobal(quad.topRight),mapFromGlobal(quad.bottomRight),mapFromGlobal(quad.bottomLeft)}));
	};	

	if (mHitWindow) {
		drawQuadF(mHitWindow->getGlobalQuad());
	}
}

void QWindow3DEditor::mousePressEvent(QMouseEvent* event)
{
	mClickPos = event->pos();
	if (mCurrentWindow != mHitWindow) {
		setCurrentWindow(mHitWindow);
	}
	mHitWindow = nullptr;
	Q_EMIT asClicked();
	QWidget::mousePressEvent(event);
}

void QWindow3DEditor::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		for (int i = 0; i < 4; i++) {
			mVertex[i]->setGlobalPos(mVertex[i]->getGlobalPos() + event->pos() - mClickPos);
		}
		notifyQuadChanged();
		update();
	}
	else {
		QWindow3D* hit = nullptr;
		for (auto window : QWindow3D::Instances) {
			if (window->getGlobalQuad().contains(event->globalPosition())) {
				hit = window;
				break;
			}
		}
		if (hit != mHitWindow) {
			mHitWindow = hit;
			update();
		}
		setCursor(mHitWindow == nullptr ? Qt::ArrowCursor : Qt::ClosedHandCursor);
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
