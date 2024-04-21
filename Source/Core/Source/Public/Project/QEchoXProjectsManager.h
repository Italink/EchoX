#ifndef QEchoXProjectsManager_h__
#define QEchoXProjectsManager_h__

#include <QObject>
#include <QHash>
#include <QMap>
#include "QEchoXProject.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QEchoXProjectsManager : public QObject {
	Q_OBJECT
public:
	struct ItemTypeInfo{
		QString name;
		QString category;
		const QMetaObject* metaObject;
	};
	inline static QString ProjectSuffix = "echox";
	static QEchoXProjectsManager& Get();
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

	IEchoXItem* createItemByName(const QString& inItemTypeName);
	const QList<ItemTypeInfo>& getItemTypeInfos();

	void registerItemType(const QMetaObject* inMetaObject,const QString& inCategory = "Common");
	void unregisterItemType(const QMetaObject* inMetaObject);
Q_SIGNALS:
	void asItemTypeInfoChanged();
	void asProjectsChanged();
	void asProjectCreated(QEchoXProject*);
	void asProjectRemoved(QEchoXProject*);
	void asCurrentProjectChanged();
	void asCurrentProjectComponentChanged();
private:
	QEchoXProjectsManager();
	void ensureProjectDir();
	void addProject(QEchoXProject* inProject);
private:
	QDir mProjectDir = QDir("./Projects");
	QList<ItemTypeInfo> mItemTypeList;
	QList<QEchoXProject*> mProjectList;
	QEchoXProject* mCurrentProject = nullptr;
};

#endif // QEchoXProjectsManager_h__
