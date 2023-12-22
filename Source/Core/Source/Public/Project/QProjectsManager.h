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
	struct ComponentTypesInfo{
		QString name;
		QString category;
		const QMetaObject* metaObject;
	};
	inline static QString ProjectSuffix = "echox";
	static QProjectsManager& Get();
	void loadProjects();

	QEchoXProject* createProject(QString inName);
	void removeProject(QEchoXProject* inProject);

	QDir getProjectsDir() const;
	QString makeUniqueName(QString inName) const;
	const QList<QEchoXProject*>& getProjectList();

	void setCurrentProject(QEchoXProject* inProject);
	QEchoXProject* getCurrentProject();

	bool saveProject(QEchoXProject* inProject);
	QEchoXProject* loadProjectOnlyHeader(QFile file);
	bool loadProjectFull(QEchoXProject* inProject);

	IEchoXComponent* createItemByName(const QString& inItemTypeName);
	QList<ComponentTypesInfo> getComponentTypeInfos();

	void registerItemType(const QMetaObject* inMetaObject,const QString& inCategory = "Common");
	void unregisterItemType(const QMetaObject* inMetaObject);
Q_SIGNALS:
	void asComponentTypeInfoChanged();
	void asProjectsChanged();
	void asProjectCreated(QEchoXProject*);
	void asProjectRemoved(QEchoXProject*);
	void asCurrentProjectChanged();
	void asCurrentProjectComponentChanged();
private:
	QProjectsManager();
	void ensureProjectDir();
	void addProject(QEchoXProject* inProject);
private:
	QDir mProjectDir = QDir("./Projects");
	QMap<QString, ComponentTypesInfo> mComponentTypeInfoMap;
	QList<QEchoXProject*> mProjectList;
	QEchoXProject* mCurrentProject = nullptr;
};

#endif // QProjectsManager_h__
