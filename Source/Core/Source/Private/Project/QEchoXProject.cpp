#include "QEchoXProject.h"
#include "QEchoXProjectsManager.h"
#include "Serialization.h"

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
	QString name = QEchoXProjectsManager::Get().makeUniqueName(inNewName);
	return getProjectFile().rename(QEchoXProjectsManager::Get().getProjectsDir().filePath(name + "." + QEchoXProjectsManager::ProjectSuffix));
}

QFile QEchoXProject::getProjectFile() const
{
	return QEchoXProjectsManager::Get().getProjectsDir().filePath(getProjectName() + "." + QEchoXProjectsManager::ProjectSuffix);
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
	return QEchoXProjectsManager::Get().saveProject(this);
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

const QList<IEchoXComponent*>& QEchoXProject::getComponents() const
{
	return mComponents;
}

void QEchoXProject::clearInvailedItem()
{
	mComponents.removeAll(nullptr);
}
