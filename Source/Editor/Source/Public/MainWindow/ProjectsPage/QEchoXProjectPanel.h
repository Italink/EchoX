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
	void setProject(IEchoXProject* inProject);
	IEchoXProject* getProject() const;
Q_SIGNALS:
	void asReturnBack();
private:
	void closeEvent(QCloseEvent* e) override;
private:
	IEchoXProject* mProject;
	QPushButton* mBtReturn;
	QWidget* mCurrProjectWidget = nullptr;
};

#endif // QEchoXProjectPanel_h__
