#ifndef QEchoXProjectModel_h__
#define QEchoXProjectModel_h__

#include <QTreeWidget>
#include <QObject>
#include <QMap>
#include <QAbstractItemModel>
#include "Project/QEchoXProject.h"

class QEchoXProjectModel : public QAbstractItemModel {
    Q_OBJECT
        enum Roles {
        name = 0,
    };
public:
    QEchoXProjectModel(QObject* parent = 0);
    ~QEchoXProjectModel() {}
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    void refresh();
    Q_INVOKABLE IEchoXItem* getComponentByIndex(int index);
private:
	QEchoXProject* mCurrentProject = nullptr;
};

#endif // QEchoXProjectModel_h__
