#ifndef QEchoXProjectPanel_h__
#define QEchoXProjectPanel_h__

#include <QWidget>
#include <QListWidget>
#include <QWheelEvent>
#include <QSplitter>
#include "Project/QProjectsManager.h"
#include "DetailView/QDetailView.h"

class QPushButton;

class QEchoXProjectPanel: public QWidget {
	Q_OBJECT
public:
	QEchoXProjectPanel();
	void setProject(QEchoXProject* inProject);
	QEchoXProject* getProject() const;
Q_SIGNALS:
	void asReturnBack();
private:
	QEchoXProject* mProject;
	QPushButton* mBtReturn;
	QDetailView* mDetailView;
	QWidget* mAssetView;
	QWidget* mOutline;
	QSplitter* mSplitter;
};

#endif // QEchoXProjectPanel_h__
