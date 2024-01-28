#include "QEchoXProjectModel.h"
#include "QEchoXController.h"
#include "Project/QEchoXProjectsManager.h"

QEchoXProjectModel::QEchoXProjectModel(QObject* parent)
	: QAbstractItemModel(parent) {
    connect(&QEchoXProjectsManager::Get(), &QEchoXProjectsManager::asCurrentProjectChanged, this, &QEchoXProjectModel::refresh);
}

QVariant QEchoXProjectModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    IEchoXComponent* component = static_cast<IEchoXComponent*>(index.internalPointer());
    return component->metaObject()->className();
}

Qt::ItemFlags QEchoXProjectModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex QEchoXProjectModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    if (!parent.isValid() && mCurrentProject) {
        return createIndex(row, column, mCurrentProject->getComponents()[row]);
    }
    return QModelIndex();
}

QModelIndex QEchoXProjectModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int QEchoXProjectModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid() && mCurrentProject)
        return mCurrentProject->getComponents().size();
    return 0;
}

void QEchoXProjectModel::refresh() {
    beginResetModel();
    if (mCurrentProject) {
        mCurrentProject->disconnect(this);
    }
    mCurrentProject = QEchoXProjectsManager::Get().getCurrentProject();
    connect(mCurrentProject, &QEchoXProject::asComponentsChanged, this, &QEchoXProjectModel::refresh);
    endResetModel();
}

IEchoXComponent* QEchoXProjectModel::getComponentByIndex(int index)
{
    return mCurrentProject->getComponents().value(index);
}

int QEchoXProjectModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QHash<int, QByteArray> QEchoXProjectModel::roleNames() const {
    return {
        { Roles::name,"name" },
    };
}
