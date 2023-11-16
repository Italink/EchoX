#ifndef QProjectsManager_h__
#define QProjectsManager_h__

#include <QObject>
#include <QHash>
#include <QMap>
#include "QEchoXProject.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QProjectsManager : public QObject {
	Q_OBJECT
public:
	static QProjectsManager& Get();

	void loadProjects();

	QEchoXProject* createProject(QString inName);
	void removeProject(QEchoXProject* inProject);

	QDir getProjectsDir() const;
	QString makeUniqueName(QString inName) const;
	const QList<QEchoXProject*>& getProjectList();

	void setCurrentProject(QEchoXProject* inProject);
	QEchoXProject* getCurrentProject();
Q_SIGNALS:
	void asProjectCreated(QEchoXProject*);
	void asProjectRemoved(QEchoXProject*);
	void asCurrrentProjectChanged(QEchoXProject*);
private:
	QProjectsManager();
	void addProject(QEchoXProject* inProject);
private:
	QDir mProjectDir = QDir("./Projects");
	QList<QEchoXProject*> mProjectList;
	QMap<QString, QEchoXProject*> mProjectsMap;
	QEchoXProject* mCurrentProject = nullptr;
};

#endif // QProjectsManager_h__
