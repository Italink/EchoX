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

void QEchoXProject::activate()
{
	clearInvailedItem();
	bActivated = true;
	for (auto& item : mComponents)
		item->activateInternal();
}

void QEchoXProject::deactivate()
{
	clearInvailedItem();
	for (auto& item : mComponents)
		item->deactivateInternal();
	bActivated = false;
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

QList<IEchoXComponent*> QEchoXProject::getItems() const
{
	return mComponents;
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

void QEchoXProject::setItems(QList<IEchoXComponent*> val)
{
	mComponents = val;
}

void QEchoXProject::setThumbnail(QPixmap inPixmap)
{
	mThumbnail = inPixmap.scaled(QSize(400, 400));
}

void QEchoXProject::addComponent(IEchoXComponent* inItem)
{
	mComponents << inItem;
	inItem->setParent(this);
	Q_EMIT asComponentsChanged();
	if (bActivated) {
		inItem->activateInternal();
	}
}

void QEchoXProject::removeComponent(IEchoXComponent* inItem)
{
	if (mComponents.removeOne(inItem)) {
		Q_EMIT asComponentsChanged();
	}
}

void QEchoXProject::clearInvailedItem()
{
	mComponents.removeAll(nullptr);
}
