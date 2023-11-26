#include "QEchoXProject.h"
#include "QProjectsManager.h"
#include "Serialization.h"
#include "DetailView/QDetailView.h"

QEchoXProject::QEchoXProject()
{
}

QString QEchoXProject::getProjectName() const
{
	return objectName();
}

bool QEchoXProject::rename(QString inNewName)
{
	QString name = QProjectsManager::Get().makeUniqueName(inNewName);
	return getProjectFile().rename(QProjectsManager::Get().getProjectsDir().filePath(name + "." + QProjectsManager::ProjectSuffix));
}

QFile QEchoXProject::getProjectFile() const
{
	return QProjectsManager::Get().getProjectsDir().filePath(getProjectName() + "." + QProjectsManager::ProjectSuffix);
}

QList<IEchoXItem*> QEchoXProject::getItems() const
{
	return mItems;
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

bool QEchoXProject::save()
{
	return QProjectsManager::Get().saveProject(this);
}

void QEchoXProject::setItems(QList<IEchoXItem*> val)
{
	mItems = val;
}

void QEchoXProject::setThumbnail(QPixmap inPixmap)
{
	mThumbnail = inPixmap.scaled(QSize(400, 400));
}

void QEchoXProject::addItem(IEchoXItem* inItem)
{
	mItems << inItem;
	inItem->setParent(this);
	Q_EMIT asItemsChanged();
}

void QEchoXProject::removeItem(IEchoXItem* inItem)
{
	if (mItems.removeOne(inItem)) {
		Q_EMIT asItemsChanged();
	}
}
