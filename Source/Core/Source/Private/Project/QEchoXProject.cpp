#include "QEchoXProject.h"
#include "QProjectsManager.h"

QEchoXProject::QEchoXProject(QString inName, Type inType)
	: mType(inType)
{
	setObjectName(inName);
}

QString QEchoXProject::getProjectName() const
{
	return objectName();
}

bool QEchoXProject::rename(QString inNewName)
{
	QString name = QProjectsManager::Get().makeUniqueName(inNewName);
	QDir oldDir = getProjectDir();
	setObjectName(name);
	QDir newDir = getProjectDir();
	return oldDir.rename(oldDir.path(), newDir.path());
}

QFile QEchoXProject::getProjectConfigFile() const
{
	return getProjectDir().filePath(ConfgiFileName);
}

QDir QEchoXProject::getProjectDir() const
{
	QDir dir = QProjectsManager::Get().getProjectsDir();
	return dir.filePath(getProjectName());
}

bool QEchoXProject::makeProjectDir()
{
	QDir dir = getProjectDir();
	if (!dir.mkpath("Resources")) {
		return false;
	}

	return saveProject();
}

bool QEchoXProject::removeProjectDir()
{
	QDir dir = getProjectDir();;
	return dir.rmpath(dir.path());
}

QPixmap QEchoXProject::getThumbnail()
{
	static QPixmap EmptyThumbnail;
	if (EmptyThumbnail.isNull()){
		EmptyThumbnail = QPixmap(400, 400);
		EmptyThumbnail.fill(Qt::black);
	}
	return mThumbnail.isNull() ? EmptyThumbnail: mThumbnail;
}

bool QEchoXProject::saveProject()
{
	QFile file = getProjectConfigFile();
	if (file.open(QFile::WriteOnly)) {
		file.write("{}");
		return true;
	}
	return false;
}
