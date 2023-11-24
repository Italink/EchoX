#include "QProjectsManager.h"
#include "Serialization.h"
#include <QMetaMethod>
#include "LoggingCategory.h"
#include "QEchoXWidgetsProject.h"

QProjectsManager& QProjectsManager::Get()
{
	static QProjectsManager Instance;
	return Instance;
}

void QProjectsManager::loadProjects()
{
	for (const QFileInfo& fileInfo : getProjectsDir().entryInfoList(QDir::Filter::Files | QDir::Filter::NoDotAndDotDot)) {
		QString suffix = fileInfo.suffix();
		if (suffix.compare(ProjectSuffix, Qt::CaseInsensitive) == 0) {
			IEchoXProject* project = loadProjectOnlyHeader(fileInfo.filePath());
			addProject(project);
		}
	}
	Q_EMIT asProjectsChanged();
}

IEchoXProject* QProjectsManager::createProject(QString inName)
{
	if (inName.isEmpty())
		inName = "NewProject";
	IEchoXProject* project = new QEchoXWidgetsProject();
	project->setObjectName(makeUniqueName(inName));
	addProject(project);
	project->save();
	Q_EMIT asProjectCreated(project);
	return project;
}

QProjectsManager::QProjectsManager()
{
	registerProjectType(&QEchoXWidgetsProject::staticMetaObject);
}

void QProjectsManager::addProject(IEchoXProject* inProject)
{
	if (inProject) {
		mProjectList << inProject;
	}
}

void QProjectsManager::removeProject(IEchoXProject* inProject)
{
	mProjectList.removeOne(inProject);
	Q_EMIT asProjectRemoved(inProject);
	inProject->getProjectFile().remove();
	inProject->deleteLater();
}

QDir QProjectsManager::getProjectsDir() const
{
	return mProjectDir;
}

QString QProjectsManager::makeUniqueName(QString inName) const
{
	QSet<QString> nameSet;
	for (auto proj : mProjectList) {
		nameSet << proj->getProjectName();
	}
	if (!nameSet.contains(inName))
		return inName;
	QString newName;
	int index = 0;
	do {
		newName = inName + QString::number(index++);
	} while (nameSet.contains(newName));
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
		loadProjectFull(mCurrentProject);
		Q_EMIT asCurrrentProjectChanged(mCurrentProject);
	}
}

IEchoXProject* QProjectsManager::getCurrentProject()
{
	return mCurrentProject;
}

bool QProjectsManager::saveProject(IEchoXProject* inProject)
{
	Q_ASSERT(inProject);
	QFile file = inProject->getProjectFile();
	if (file.open(QFile::WriteOnly)) {
		QDataStream stream(&file);
		QCborMap header;
		header.insert(QString("ProjectType"), inProject->metaObject()->className());
		QByteArray thumbnail;
		QDataStream imageStream(&thumbnail, QDataStream::WriteOnly);
		imageStream << inProject->getThumbnail();
		header.insert(QString("Thumbnail"), thumbnail);
		stream << header;
		stream << Serialization::toCbor(inProject);
		return true;
	}
	return false;
}

IEchoXProject* QProjectsManager::loadProjectOnlyHeader(QFile file)
{
	if (file.open(QFile::ReadOnly)) {
		QDataStream stream(&file);
		QCborMap header;
		stream >> header;
		QString projectType = header.value(QString("ProjectType")).toString();
		QByteArray thumbnail = header.value(QString("Thumbnail")).toByteArray();
		QDataStream imageStream(&thumbnail, QDataStream::ReadOnly);
		QPixmap pixmap;
		imageStream >> pixmap;
		if (const QMetaObject* metaObject = mProjectTypeMap.value(projectType, nullptr)) {
			QObject* object = metaObject->newInstance();
			if (IEchoXProject* project = qobject_cast<IEchoXProject*>(object)) {
				QFileInfo fileInfo(file);
				project->setObjectName(fileInfo.fileName());
				project->setThumbnail(pixmap);
				return project;
			}
		}
	}
	return nullptr;
}

bool QProjectsManager::loadProjectFull(IEchoXProject* inProject)
{
	if (!inProject)
		return false;
	QFile file = inProject->getProjectFile();
	if (file.open(QFile::ReadOnly)) {
		QDataStream stream(&file);
		QCborMap header;
		QCborMap body;
		stream >> header;
		stream >> body;
		Serialization::fromCbor(inProject, body);
		return true;
	}
	return false;
}

void QProjectsManager::registerProjectType(const QMetaObject* inMetaObject)
{
	mProjectTypeMap.insert(inMetaObject->className(), inMetaObject);
	bool isInvaild = false;
	for (int i = 0; i < inMetaObject->constructorCount(); i++) {
		QMetaMethod method = inMetaObject->constructor(i);
		if (method.parameterCount() == 0) {
			isInvaild = true;
		}
	}
	if (!isInvaild) {
		qCFatal(EchoX) << "must use Q_INVOKABLE";
	}
}

void QProjectsManager::unregisterProjectType(const QMetaObject* inMetaObject)
{
	mProjectTypeMap.remove(inMetaObject->className());
}

