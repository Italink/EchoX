#ifndef QEchoXProjectsPanel_h__
#define QEchoXProjectsPanel_h__

#include <QWidget>
#include <QListWidget>
#include <QWheelEvent>
#include <QPushButton>
#include "Project/QProjectsManager.h"

class QEchoXProjectsPanel : public QWidget {
	Q_OBJECT
public:
	QEchoXProjectsPanel();
	void refreshProjects();
	void setIconScaleFactor(float inVar);
	QRect getProjectGemotry(QEchoXProject* inProject);
Q_SIGNALS:
	void asProjectDoubleClicked(QEchoXProject*);
private:
	QListWidgetItem* getProjectItem(QEchoXProject* inProject);
	void updateProjectItem(QEchoXProject* inProject);
	void addProject(int index, QEchoXProject* inProject);
	void removeProject(QEchoXProject* inProject);
	void onCreateNewProject();
	void onItemDoubleClicked(QListWidgetItem* inItem);
	void refreshIconSize();
protected:
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	QPushButton* mBtCreateNew;
	QListWidget* mListWidget;
	QMap<QEchoXProject*, QListWidgetItem*> mProjectItemMap;
	float mTextHeight = 20;
	float mIconWdith = 80;
	float mIconAspectRatio = 1.0f;
	float mIconScaleFactor = 1.0f;
};

#endif // QEchoXProjectsPanel_h__
