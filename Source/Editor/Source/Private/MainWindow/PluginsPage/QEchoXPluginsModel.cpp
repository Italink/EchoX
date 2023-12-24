#include "QEchoXPluginsModel.h"

QEchoXPluginsModel::QEchoXPluginsModel(QObject* parent)
    : QAbstractItemModel(parent) {
}

QEchoXPluginsModel::~QEchoXPluginsModel() {
}

QVariant QEchoXPluginsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    QEnginePluginHandler* handler = static_cast<QEnginePluginHandler*>(index.internalPointer());
    if (role == Roles::name) {
        return handler->info.name;
    }
    else if (role == Roles::icon) {
        return handler->info.icon;
    }
    else if (role == Roles::author) {
        return handler->info.author;
    }
    else if (role == Roles::link) {
        return handler->info.link;
    }
    else if (role == Roles::desc) {
        return handler->info.description;
    }
    else if (role == Roles::images) {
        return  QVariant::fromValue(handler->info.images);
    }
    return QVariant();
}

QModelIndex QEchoXPluginsModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    if (!parent.isValid()) {
        return createIndex(row, column, &QEnginePluginManager::Get().getPluginHandlers()[row]);
    }
    return QModelIndex();
}

QModelIndex QEchoXPluginsModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

Qt::ItemFlags QEchoXPluginsModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int QEchoXPluginsModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid())
        return QEnginePluginManager::Get().getPluginHandlers().size();
    return 0;
}

QHash<int, QByteArray> QEchoXPluginsModel::roleNames() const {
    return {
        { Roles::name,"name" },
        { Roles::icon,"icon" },
        { Roles::author,"author" },
        { Roles::link,"link" },
        { Roles::desc,"desc" },
        { Roles::images,"images" },
    };
}

int QEchoXPluginsModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

void QEchoXPluginsModel::refresh() {
    beginResetModel();
    endResetModel();
}
