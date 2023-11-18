#include "IEchoXProject.h"
#include "QProjectsManager.h"
#include "Serialization.h"
#include "DetailView/QDetailView.h"

IEchoXProject::IEchoXProject()
{
}

QString IEchoXProject::getProjectName() const
{
	return objectName();
}

bool IEchoXProject::rename(QString inNewName)
{
	QString name = QProjectsManager::Get().makeUniqueName(inNewName);
	return getProjectFile().rename(QProjectsManager::Get().getProjectsDir().filePath(name + "." + QProjectsManager::ProjectSuffix));
}

QFile IEchoXProject::getProjectFile() const
{
	return QProjectsManager::Get().getProjectsDir().filePath(getProjectName() + "." + QProjectsManager::ProjectSuffix);
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

QWidget* IEchoXProject::createProjectPanel()
{
	QDetailView* detailView = new QDetailView;
	detailView->setObject(this);
	return detailView;
}

bool IEchoXProject::save()
{
	return QProjectsManager::Get().saveProject(this);
}

void IEchoXProject::setThumbnail(QPixmap inPixmap)
{
	mThumbnail = inPixmap.scaled(QSize(400, 400));
}
