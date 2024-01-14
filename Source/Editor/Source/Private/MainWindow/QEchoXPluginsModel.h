#ifndef QEchoXPluginsModel_h__
#define QEchoXPluginsModel_h__

#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QQmlContext>
#include <QAbstractItemModel>
#include "Plugin/QEnginePluginManager.h"

class QEchoXPluginsModel : public QAbstractItemModel {
    enum Roles {
        name = 0,
        icon,
        author,
        link,
        desc,
        images
    };
public:
    QEchoXPluginsModel(QObject* parent = 0);
    ~QEchoXPluginsModel();
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    void refresh();
};

#endif // QEchoXPluginsModel_h__
