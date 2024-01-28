#ifndef QEchoXComponentModel_h__
#define QEchoXComponentModel_h__

#include <QTreeWidget>
#include <QObject>
#include <QMap>
#include <QAbstractItemModel>
#include "Project/QEchoXProjectsManager.h"

class QEchoXComponentModel : public QAbstractItemModel {
    Q_OBJECT
        enum Roles {
        name = 0,
    };
public:
    QEchoXComponentModel(QObject* parent = 0);
    ~QEchoXComponentModel() {}
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    void refresh();

    Q_INVOKABLE void notifyBeginDrag(int index);
Q_SIGNALS:
	void asItemDropped(QPoint, IEchoXComponent*);
};

#endif // QEchoXComponentModel_h__
