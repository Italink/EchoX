#ifndef QProjectsManager_h__
#define QProjectsManager_h__

#include <QObject>
#include <QHash>
#include <QMap>
#include "IEchoXProject.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QProjectsManager : public QObject {
	Q_OBJECT
public:
	inline static QString ProjectSuffix = ".echox";
	static QProjectsManager& Get();
	void loadProjects();

	IEchoXProject* createProject(QString inName);
	void removeProject(IEchoXProject* inProject);

	QDir getProjectsDir() const;
	QString makeUniqueName(QString inName) const;
	const QList<IEchoXProject*>& getProjectList();

	void setCurrentProject(IEchoXProject* inProject);
	IEchoXProject* getCurrentProject();

	void saveProject(IEchoXProject* inProject);
	void loadProject(QFile file);
Q_SIGNALS:
	void asProjectCreated(IEchoXProject*);
	void asProjectRemoved(IEchoXProject*);
	void asCurrrentProjectChanged(IEchoXProject*);
private:
	QProjectsManager();
	void addProject(IEchoXProject* inProject);
private:
	QDir mProjectDir = QDir("./Projects");
	QList<IEchoXProject*> mProjectList;
	QMap<QString, IEchoXProject*> mProjectsMap;
	IEchoXProject* mCurrentProject = nullptr;
};

#endif // QProjectsManager_h__
