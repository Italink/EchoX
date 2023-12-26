#ifndef QEchoXProjectsModel_h__
#define QEchoXProjectsModel_h__

#include <QTreeWidget>
#include <QObject>
#include <QMap>
#include <QAbstractItemModel>
#include "DetailView/QDetailView.h"
#include "Project/QProjectsManager.h"

class QEchoXProjectsModel : public QAbstractItemModel {
    Q_OBJECT
        enum Roles {
        name = 0,
    };
public:
    QEchoXProjectsModel(QObject* parent = 0);
    ~QEchoXProjectsModel() {}
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    void refresh();
    Q_INVOKABLE void notifyProjectDoubleClicked(QModelIndex index);
    Q_INVOKABLE void createNewProject();
Q_SIGNALS:
private:
    QList<QEchoXProject*> mProjectList;
};

#endif // QEchoXProjectsModel_h__
