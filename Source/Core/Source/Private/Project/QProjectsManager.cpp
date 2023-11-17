#include "QProjectsManager.h"

QProjectsManager& QProjectsManager::Get()
{
	static QProjectsManager Instance;
	return Instance;
}

void QProjectsManager::loadProjects()
{
	for (const QFileInfo& fileInfo : getProjectsDir().entryInfoList(QDir::Filter::Files | QDir::Filter::NoDotAndDotDot)) {
		if (fileInfo.suffix().compare(ProjectSuffix, Qt::CaseInsensitive) == 0) {
			IEchoXProject* project = new IEchoXProject(fileInfo.fileName());
			addProject(project);
		}
	}
}

IEchoXProject* QProjectsManager::createProject(QString inName)
{
	if (inName.isEmpty())
		inName = "NewProject";
	IEchoXProject* project = new IEchoXProject(makeUniqueName(inName));
	addProject(project);
	project->saveProject();
	Q_EMIT asProjectCreated(project);
	return project;
}

QProjectsManager::QProjectsManager()
{

}

void QProjectsManager::addProject(IEchoXProject* inProject)
{
	mProjectList << inProject;
	mProjectsMap.insert(inProject->objectName(), inProject);
}

void QProjectsManager::removeProject(IEchoXProject* inProject)
{
	mProjectList.removeOne(inProject);
	mProjectsMap.remove(inProject->objectName());
	Q_EMIT asProjectRemoved(inProject);
}

QDir QProjectsManager::getProjectsDir() const
{
	return mProjectDir;
}

QString QProjectsManager::makeUniqueName(QString inName) const
{
	if (!mProjectsMap.contains(inName))
		return inName;
	QString newName;
	int index = 0;
	do {
		newName = inName + QString::number(index++);
	} while (mProjectsMap.contains(newName));
	return newName;
}

const QList<IEchoXProject*>& QProjectsManager::getProjectList()
{
	return mProjectList;
}

void QProjectsManager::setCurrentProject(IEchoXProject* inProject)
{
	if (mCurrentProject != inProject) {
		mCurrentProject = inProject;
		Q_EMIT asCurrrentProjectChanged(mCurrentProject);
	}
}

IEchoXProject* QProjectsManager::getCurrentProject()
{
	return mCurrentProject;
}

void QProjectsManager::saveProject(IEchoXProject* inProject)
{

}

void QProjectsManager::loadProject(QFile file)
{

}

