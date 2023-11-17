#include "IEchoXProject.h"
#include "QProjectsManager.h"
#include "Serialization.h"

IEchoXProject::IEchoXProject(QString inName)
{
	setObjectName(inName);
}

QString IEchoXProject::getProjectName() const
{
	return objectName();
}

bool IEchoXProject::rename(QString inNewName)
{
	QString name = QProjectsManager::Get().makeUniqueName(inNewName);
	return getProjectFile().rename(QProjectsManager::Get().getProjectsDir().filePath(name + QProjectsManager::ProjectSuffix));
}

QFile IEchoXProject::getProjectFile() const
{
	return QProjectsManager::Get().getProjectsDir().filePath(getProjectName() + QProjectsManager::ProjectSuffix);
}

QPixmap IEchoXProject::getThumbnail()
{
	static QPixmap EmptyThumbnail;
	if (EmptyThumbnail.isNull()){
		EmptyThumbnail = QPixmap(400, 400);
		EmptyThumbnail.fill(Qt::black);
	}
	return mThumbnail.isNull() ? EmptyThumbnail: mThumbnail;
}

bool IEchoXProject::saveProject()
{
	QFile file = getProjectFile();
	if (file.open(QFile::WriteOnly)) {
		QDataStream stream(&file);
		stream << Serialization::toCbor(this);
		return true;
	}
	return false;
}
