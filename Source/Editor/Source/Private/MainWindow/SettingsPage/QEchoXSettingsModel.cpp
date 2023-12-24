#include "QEchoXSettingsModel.h"

QEchoXSettingsModel::QEchoXSettingsModel(QObject* parent)
	: QAbstractItemModel(parent) {
}

QVariant QEchoXSettingsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    IEchoSettingsTreeNode* node = static_cast<IEchoSettingsTreeNode*>(index.internalPointer());
    return node->getName();
}

Qt::ItemFlags QEchoXSettingsModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex QEchoXSettingsModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    if (!parent.isValid()) {
        return createIndex(row, column, mCategoryList[row].get());
    }
    return createIndex(row, column, mCategoryList[parent.row()]->mChildren[row].get());
}

QModelIndex QEchoXSettingsModel::parent(const QModelIndex& index) const {
    if (!index.isValid())
        return QModelIndex();
    IEchoSettingsTreeNode* node = static_cast<IEchoSettingsTreeNode*>(index.internalPointer());
    if (node->mParent) {
        return createIndex(mCategoryList.indexOf(node->mParent), 0, node->mParent);
    }
    return QModelIndex();
}

int QEchoXSettingsModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid())
        return mCategoryList.size();
    IEchoSettingsTreeNode* node = static_cast<IEchoSettingsTreeNode*>(parent.internalPointer());
    return node->mChildren.count();
}

void QEchoXSettingsModel::refresh() {
    beginResetModel();
    mCategoryList.clear();
    const QList<IEchoXSettings*>& allSettings = QSettingsManager::Get().getAllSettings();
    QMap<QString, EchoSettingsTreeNode_Categroy*> categroyMap;
    for (IEchoXSettings* settings : allSettings) {
        QString category = settings->category();
        if (!categroyMap.contains(category)) {
            QSharedPointer<EchoSettingsTreeNode_Categroy> newCategroy(new EchoSettingsTreeNode_Categroy(category));
            categroyMap[category] = newCategroy.get();
            mCategoryList << newCategroy;
        }
        categroyMap[category]->addSettings(settings);
    }
    endResetModel();
    Q_EMIT settingsChanged();
}

void QEchoXSettingsModel::notifyCurrentSettingChanged(QModelIndex index)
{
	IEchoSettingsTreeNode* node = static_cast<IEchoSettingsTreeNode*>(index.internalPointer());
	Q_EMIT settingsSelected(node->getSettings());
}

int QEchoXSettingsModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QHash<int, QByteArray> QEchoXSettingsModel::roleNames() const {
    return {
        { Roles::name,"name" },
    };
}
