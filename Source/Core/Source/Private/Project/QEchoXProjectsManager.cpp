#include "QEchoXProjectsManager.h"
#include "Serialization.h"
#include <QMetaMethod>
#include "LoggingCategory.h"

QEchoXProjectsManager& QEchoXProjectsManager::Get()
{
	static QEchoXProjectsManager Instance;
	return Instance;
}

void QEchoXProjectsManager::loadProjects()
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

QEchoXProject* QEchoXProjectsManager::createProject(QString inName)
{
	if (inName.isEmpty())
		inName = "NewProject";
	QEchoXProject* project = new QEchoXProject();
	project->setObjectName(makeUniqueName(inName));
	addProject(project);
	project->save();
	Q_EMIT asProjectCreated(project);
	Q_EMIT asProjectsChanged();
	return project;
}

QEchoXProjectsManager::QEchoXProjectsManager()
{
}

void QEchoXProjectsManager::ensureProjectDir()
{
	QDir dir = getProjectsDir();
	if (!dir.exists())
		dir.mkpath(".");
}

void QEchoXProjectsManager::addProject(QEchoXProject* inProject)
{
	if (inProject) {
		mProjectList << inProject;
		inProject->setParent(this);
	}
}

void QEchoXProjectsManager::removeProject(QEchoXProject* inProject)
{
	mProjectList.removeOne(inProject);
	Q_EMIT asProjectRemoved(inProject);
	Q_EMIT asProjectsChanged();
	inProject->getProjectFile().remove();
	inProject->setParent(nullptr);
	inProject->deleteLater();
}

QDir QEchoXProjectsManager::getProjectsDir() const
{
	return mProjectDir;
}

QString QEchoXProjectsManager::makeUniqueName(QString inName) const
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

const QList<QEchoXProject*>& QEchoXProjectsManager::getProjectList()
{
	return mProjectList;
}

void QEchoXProjectsManager::setCurrentProject(QEchoXProject* inProject)
{
	if (mCurrentProject != inProject) {
		if (mCurrentProject) {
			mCurrentProject->deactivate();
			mCurrentProject->disconnect(this);
		}
		mCurrentProject = inProject;
		if (mCurrentProject) {
			loadProjectFull(mCurrentProject);
			mCurrentProject->activate();
			Q_EMIT asCurrentProjectChanged();
			connect(mCurrentProject, &QEchoXProject::asComponentsChanged, this, &QEchoXProjectsManager::asCurrentProjectComponentChanged);
		}
	}
}

QEchoXProject* QEchoXProjectsManager::getCurrentProject()
{
	return mCurrentProject;
}

bool QEchoXProjectsManager::saveProject(QEchoXProject* inProject)
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

QEchoXProject* QEchoXProjectsManager::loadProjectOnlyHeader(QFile file)
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

bool QEchoXProjectsManager::loadProjectFull(QEchoXProject* inProject)
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
		qCDebug(EchoX) << body;
		Serialization::fromCbor(inProject, body);
		return true;
	}
	return false;
}

IEchoXComponent* QEchoXProjectsManager::createComponentByName(const QString& inComponentTypeName)
{
	for (auto& componentType : mComponentTypeList) {
		if (componentType.name == inComponentTypeName) {
			return qobject_cast<IEchoXComponent*>(componentType.metaObject->newInstance());
		}
	}
	return nullptr;
}

const QList<QEchoXProjectsManager::ComponentTypesInfo>& QEchoXProjectsManager::getComponentTypeInfos()
{
	return mComponentTypeList;
}

void QEchoXProjectsManager::registerComponentType(const QMetaObject* inMetaObject, const QString& inCategory)
{
	qRegisterMetaType(inMetaObject->metaType());
	ComponentTypesInfo info;
	info.name = inMetaObject->className();
	info.category = inCategory;
	info.metaObject = inMetaObject;
	mComponentTypeList << info;
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
	Q_EMIT asComponentTypeInfoChanged();
}

void QEchoXProjectsManager::unregisterComponentType(const QMetaObject* inMetaObject)
{
	mComponentTypeList.removeIf([inMetaObject](const ComponentTypesInfo& info) {
		return info.metaObject == inMetaObject;
	});
}
