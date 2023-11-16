#include "QProjectsManager.h"

QProjectsManager& QProjectsManager::Get()
{
	static QProjectsManager Instance;
	return Instance;
}

void QProjectsManager::loadProjects()
{
	for (const QFileInfo& dirEntry : getProjectsDir().entryInfoList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot)) {
		QDir dir(dirEntry.filePath());
		if (QFile::exists(dir.filePath(QEchoXProject::ConfgiFileName))) {
			QEchoXProject* project = new QEchoXProject(dirEntry.fileName());
			addProject(project);
		}
	}
}

QEchoXProject* QProjectsManager::createProject(QString inName)
{
	if (inName.isEmpty())
		inName = "NewProject";
	QEchoXProject* project = new QEchoXProject(makeUniqueName(inName));
	addProject(project);
	project->makeProjectDir();
	Q_EMIT asProjectCreated(project);
	return project;
}

QProjectsManager::QProjectsManager()
{

}

void QProjectsManager::addProject(QEchoXProject* inProject)
{
	mProjectList << inProject;
	mProjectsMap.insert(inProject->objectName(), inProject);
}

void QProjectsManager::removeProject(QEchoXProject* inProject)
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

const QList<QEchoXProject*>& QProjectsManager::getProjectList()
{
	return mProjectList;
}

void QProjectsManager::setCurrentProject(QEchoXProject* inProject)
{
	if (mCurrentProject != inProject) {
		mCurrentProject = inProject;
		Q_EMIT asCurrrentProjectChanged(mCurrentProject);
	}
}

QEchoXProject* QProjectsManager::getCurrentProject()
{
	return mCurrentProject;
}

