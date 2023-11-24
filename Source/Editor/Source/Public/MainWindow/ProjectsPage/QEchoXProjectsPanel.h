#ifndef QEchoXProjectsPanel_h__
#define QEchoXProjectsPanel_h__

#include <QWidget>
#include <QListWidget>
#include <QWheelEvent>
#include <QPushButton>
#include <QLineEdit>
#include "Project/QProjectsManager.h"

class QEchoXProjectsHeader : public QWidget {
public:
	QEchoXProjectsHeader();
private:
	void paintEvent(QPaintEvent* e) override;
private:
	QPushButton* mBtCreateProject;
	QLineEdit* mSearchEdit;
};

class QEchoXProjectsPanel : public QWidget {
	Q_OBJECT
public:
	QEchoXProjectsPanel();
	void refreshProjects();
	void setIconScaleFactor(float inVar);
	QRect getProjectGemotry(IEchoXProject* inProject);
Q_SIGNALS:
	void asProjectDoubleClicked(IEchoXProject*);
private:
	QListWidgetItem* getProjectItem(IEchoXProject* inProject);
	void updateProjectItem(IEchoXProject* inProject);
	void addProject(int index, IEchoXProject* inProject);
	void removeProject(IEchoXProject* inProject);
	void onCreateNewProject();
	void onItemDoubleClicked(QListWidgetItem* inItem);
	void refreshIconSize();
protected:
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	QEchoXProjectsHeader* mHeader;
	QListWidget* mProjectListWidget;
	QMap<IEchoXProject*, QListWidgetItem*> mProjectItemMap;
	float mTextHeight = 20;
	float mIconWdith = 80;
	float mIconAspectRatio = 1.0f;
	float mIconScaleFactor = 1.0f;
};

#endif // QEchoXProjectsPanel_h__
