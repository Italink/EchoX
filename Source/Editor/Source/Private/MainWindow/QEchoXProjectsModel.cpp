#include "QEchoXProjectsModel.h"
#include "QEchoXController.h"

QEchoXProjectsModel::QEchoXProjectsModel(QObject* parent)
	: QAbstractItemModel(parent) {
    refresh();
    connect(&QProjectsManager::Get(), &QProjectsManager::asProjectsChanged, this, &QEchoXProjectsModel::refresh);
}

QVariant QEchoXProjectsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    QEchoXProject* project = static_cast<QEchoXProject*>(index.internalPointer());
    return project->getProjectName();
}

Qt::ItemFlags QEchoXProjectsModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex QEchoXProjectsModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    if (!parent.isValid()) {
        return createIndex(row, column, mProjectList[row]);
    }
    return QModelIndex();
}

QModelIndex QEchoXProjectsModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int QEchoXProjectsModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid())
        return mProjectList.size();
    return 0;
}

void QEchoXProjectsModel::refresh() {
    beginResetModel();
    mProjectList = QProjectsManager::Get().getProjectList();
    endResetModel();
}

void QEchoXProjectsModel::notifyProjectDoubleClicked(int index)
{
    qDebug() << index;
    if (QEchoXProject* project = mProjectList.value(index)) {
        QEchoXController::Get()->openProjectPage(project);
    }
}

void QEchoXProjectsModel::createNewProject()
{
    QProjectsManager::Get().createProject("NewProject");
}

int QEchoXProjectsModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QHash<int, QByteArray> QEchoXProjectsModel::roleNames() const {
    return {
        { Roles::name,"name" },
    };
}
