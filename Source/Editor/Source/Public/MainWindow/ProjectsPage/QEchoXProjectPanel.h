#ifndef QEchoXProjectPanel_h__
#define QEchoXProjectPanel_h__

#include <QWidget>
#include <QListWidget>
#include <QWheelEvent>
#include <QSplitter>
#include "Project/QProjectsManager.h"
#include "MainWindow/ProjectsPage/QEchoXComponentSelector.h"
#include "MainWindow/ProjectsPage/QEchoXComponentOutliner.h"

class QPushButton;

class QEchoXProjectPanel: public QWidget {
	Q_OBJECT
public:
	QEchoXProjectPanel();
Q_SIGNALS:
	void asReturnBack();
private:
	void closeEvent(QCloseEvent* e) override;
	void onComponentDropped(QPoint center, IEchoXComponent* inComponent);
	void onComponentSelected(IEchoXComponent* inComponent);
private:
	QPushButton* mBtReturn = nullptr;
	QEchoXComponentSelector* mComponentSelector = nullptr;
	QEchoXComponentOutliner* mComponentOutliner = nullptr;
	//QDetailView* mDetailView = nullptr;
};

#endif // QEchoXProjectPanel_h__
