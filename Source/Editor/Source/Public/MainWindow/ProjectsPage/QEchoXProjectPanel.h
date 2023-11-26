#ifndef QEchoXProjectPanel_h__
#define QEchoXProjectPanel_h__

#include <QWidget>
#include <QListWidget>
#include <QWheelEvent>
#include <QSplitter>
#include "Project/QProjectsManager.h"
#include "DetailView/QDetailView.h"

class QPushButton;

class QEchoXProjectItemListWidget: public QListWidget {
	Q_OBJECT
public:
Q_SIGNALS:
	void asItemDropped(QPoint, IEchoXItem*);
protected:
	void startDrag(Qt::DropActions supportedActions) override;
};

class QEchoXProjectPanel: public QWidget {
	Q_OBJECT
public:
	QEchoXProjectPanel();
	void setProject(QEchoXProject* inProject);
	QEchoXProject* getProject() const;
	void refreshItemList();
	void refreshOutliner();
Q_SIGNALS:
	void asReturnBack();
private:
	void closeEvent(QCloseEvent* e) override;
	void onItemDropped(QPoint center, IEchoXItem* item);
	void onCurrentItemChanged(QListWidgetItem* widgetItem);
private:
	QEchoXProject* mProject = nullptr;
	QPushButton* mBtReturn = nullptr;
	QEchoXProjectItemListWidget* mItemResouceList = nullptr;
	QListWidget* mOutliner = nullptr;
	QDetailView* mDetailView = nullptr;
};

#endif // QEchoXProjectPanel_h__
