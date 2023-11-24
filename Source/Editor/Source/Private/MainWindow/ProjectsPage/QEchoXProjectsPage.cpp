#include "QEchoXProjectsPage.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QVariantAnimation>
#include "Project/QProjectsManager.h"
#include "QEchoXProjectsPanel.h"
#include "QEchoXProjectPanel.h"

QEchoXProjectsPage::QEchoXProjectsPage()
	: mProjectsPanel(new QEchoXProjectsPanel)
	, mProjectPanel(new QEchoXProjectPanel)
	, mLayout(new QHBoxLayout(this))
	, mSwitchAnimation(new QVariantAnimation(this))
{
	mSwitchAnimation->setDuration(200);
	mSwitchAnimation->setStartValue(0.0f);
	mSwitchAnimation->setEndValue(1.0f);
	mLayout->setContentsMargins(0, 0, 0, 0);
	mLayout->addWidget(mProjectsPanel);
	connect(mProjectsPanel, &QEchoXProjectsPanel::asProjectDoubleClicked, this, &QEchoXProjectsPage::onEnterProject);
	connect(mProjectPanel, &QEchoXProjectPanel::asReturnBack, this, &QEchoXProjectsPage::onLeaveProject);
	connect(mSwitchAnimation, &QVariantAnimation::valueChanged, this, [this](QVariant var) {
		update();
	});
}

void QEchoXProjectsPage::onEnterProject(IEchoXProject* inProject)
{
	QProjectsManager::Get().setCurrentProject(inProject);
	mProjectsPanel->close();
	mProjectsPanel->setParent(nullptr);
	mLayout->removeWidget(mProjectsPanel);

	mProjectPanel->resize(size());
	mSrcRect = mProjectsPanel->getProjectGemotry(inProject);
	mDestRect = rect();
	mSrcPixmap = mProjectsPanel->grab().toImage();
	mDestPixmap = mProjectPanel->grab().toImage();

	mSwitchAnimation->start(QAbstractAnimation::KeepWhenStopped);
	mProjectPanel->setProject(inProject);

	mSwitchAnimation->disconnect(mFinishedConnection);
	mFinishedConnection = connect(mSwitchAnimation, &QVariantAnimation::finished, this, [this]() {
		mLayout->addWidget(mProjectPanel);
		mProjectPanel->show();
	});
}

void QEchoXProjectsPage::onLeaveProject()
{
	mProjectPanel->close();
	mProjectPanel->setParent(nullptr);
	mLayout->removeWidget(mProjectPanel);

	mProjectsPanel->resize(size());
	mSrcRect = rect();
	mDestRect = mProjectsPanel->getProjectGemotry(mProjectPanel->getProject());
	mSrcPixmap = mProjectPanel->grab().toImage();
	mDestPixmap = mProjectsPanel->grab().toImage();

	mSwitchAnimation->start(QAbstractAnimation::KeepWhenStopped);

	mSwitchAnimation->disconnect(mFinishedConnection);
	mFinishedConnection = connect(mSwitchAnimation, &QVariantAnimation::finished, this, [this]() {
		mLayout->addWidget(mProjectsPanel);
		mProjectsPanel->show();
	});
}

int interpolate(const qreal& from, const qreal& to, qreal progress)
{
	return qreal(from + (to - from) * progress);
}

void QEchoXProjectsPage::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	if (mSwitchAnimation->state() == QAbstractAnimation::State::Running) {
		
		float progress = mSwitchAnimation->currentValue().toFloat();

		QRectF animationArea;
		animationArea.setCoords(interpolate(mDestRect.left(), mSrcRect.left(), progress),
			interpolate(mDestRect.top(), mSrcRect.top(), progress),
			interpolate(mDestRect.right(), mSrcRect.right(), progress),
			interpolate(mDestRect.bottom(), mSrcRect.bottom(), progress ));

		QRectF imageArea(animationArea.x() * devicePixelRatioF(), animationArea.y() * devicePixelRatioF(), animationArea.width() * devicePixelRatioF(), animationArea.height() * devicePixelRatioF());
		QImage image = mSrcPixmap.copy(imageArea.toRect());

		QRectF area((mSrcRect.left() - animationArea.left() ) / (float)animationArea.width() * width() , (mSrcRect.top() - animationArea.top()) / (float)animationArea.height() * height() , mSrcRect.width() /animationArea.width() * width(), mSrcRect.height() / animationArea.height() * height());
		painter.drawImage(rect(), image);
		painter.setOpacity(progress);
		painter.drawImage(area, mDestPixmap.scaled(area.size().toSize(), Qt::AspectRatioMode::IgnoreAspectRatio, Qt::TransformationMode::SmoothTransformation));
	}
}
