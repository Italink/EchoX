#include "QProjectsManager.h"
#include "Serialization.h"
#include <QMetaMethod>
#include "LoggingCategory.h"

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
			QEchoXProject* project = loadProjectOnlyHeader(fileInfo.filePath());
			addProject(project);
		}
	}
	Q_EMIT asProjectsChanged();
}

QEchoXProject* QProjectsManager::createProject(QString inName)
{
	if (inName.isEmpty())
		inName = "NewProject";
	QEchoXProject* project = new QEchoXProject();
	project->setObjectName(makeUniqueName(inName));
	addProject(project);
	project->save();
	Q_EMIT asProjectCreated(project);
	return project;
}

QProjectsManager::QProjectsManager()
{
}

void QProjectsManager::ensureProjectDir()
{
	QDir dir = getProjectsDir();
	if (!dir.exists())
		dir.mkpath(".");
}

void QProjectsManager::addProject(QEchoXProject* inProject)
{
	if (inProject) {
		mProjectList << inProject;
		inProject->setParent(this);
	}
}

void QProjectsManager::removeProject(QEchoXProject* inProject)
{
	mProjectList.removeOne(inProject);
	Q_EMIT asProjectRemoved(inProject);
	inProject->getProjectFile().remove();
	inProject->setParent(nullptr);
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

const QList<QEchoXProject*>& QProjectsManager::getProjectList()
{
	return mProjectList;
}

void QProjectsManager::setCurrentProject(QEchoXProject* inProject)
{
	if (mCurrentProject != inProject) {
		mCurrentProject = inProject;
		loadProjectFull(mCurrentProject);
		Q_EMIT asCurrrentProjectChanged(mCurrentProject);
	}
}

QEchoXProject* QProjectsManager::getCurrentProject()
{
	return mCurrentProject;
}

bool QProjectsManager::saveProject(QEchoXProject* inProject)
{
	Q_ASSERT(inProject);
	ensureProjectDir();
	QFile file = inProject->getProjectFile();
	if (file.open(QFile::WriteOnly)) {
		QDataStream stream(&file);
		QCborMap header;
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

QEchoXProject* QProjectsManager::loadProjectOnlyHeader(QFile file)
{
	if (file.open(QFile::ReadOnly)) {
		QDataStream stream(&file);
		QCborMap header;
		stream >> header;
		QByteArray thumbnail = header.value(QString("Thumbnail")).toByteArray();
		QDataStream imageStream(&thumbnail, QDataStream::ReadOnly);
		QPixmap pixmap;
		imageStream >> pixmap;

		QEchoXProject* project = new QEchoXProject;
		QFileInfo fileInfo(file);
		project->setObjectName(fileInfo.baseName());
		project->setThumbnail(pixmap);
		return project;
	}
	return nullptr;
}

bool QProjectsManager::loadProjectFull(QEchoXProject* inProject)
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

IEchoXItem* QProjectsManager::createItemByName(const QString& inItemTypeName)
{
	if (auto meteObject = mItemTypeMap.value(inItemTypeName)) {
		return qobject_cast<IEchoXItem*>(meteObject->newInstance());
	}
	return nullptr;
}

const QMap<QString, const QMetaObject*>& QProjectsManager::getItemsMap()
{
	return mItemTypeMap;
}

void QProjectsManager::registerItemType(const QMetaObject* inMetaObject)
{
	qRegisterMetaType(inMetaObject->metaType());;
	mItemTypeMap.insert(inMetaObject->className(), inMetaObject);
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
	Q_EMIT asItemTypesChanged();
}

void QProjectsManager::unregisterItemType(const QMetaObject* inMetaObject)
{
	mItemTypeMap.remove(inMetaObject->className());
}
