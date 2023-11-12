#ifndef QEchoXProjectsPage_h__
#define QEchoXProjectsPage_h__

#include <QWidget>
#include <QListWidget>
#include <QWheelEvent>

class QEchoXProjectsPanel;
class QEchoXProjectPanel;
class QHBoxLayout;
class QVariantAnimation;
class QEchoXProject;


class QEchoXProjectsPage: public QWidget {
	Q_OBJECT
public:
	QEchoXProjectsPage();
private:
	void onEnterProject(QEchoXProject* inProject);
	void onLeaveProject();
	void paintEvent(QPaintEvent* event) override;
private:
	QEchoXProjectsPanel* mProjectsPanel;
	QEchoXProjectPanel* mProjectPanel;
	QHBoxLayout* mLayout;
	QVariantAnimation* mSwitchAnimation;
	QMetaObject::Connection mFinishedConnection;

	QRectF mSrcRect;
	QImage mSrcPixmap;

	QRectF mDestRect;
	QImage mDestPixmap;
};

#endif // QEchoXProjectsPage_h__
