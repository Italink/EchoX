#ifndef QEchoXSettingsModel_h__
#define QEchoXSettingsModel_h__

#include <QTreeWidget>
#include <QObject>
#include <QMap>
#include <QAbstractItemModel>
#include "Settings/QSettingsManager.h"
#include "Settings/IEchoXSettings.h"

class IEchoXSettings;
class IEchoSettingsTreeNode {
public:
    virtual QString getName() = 0;
    virtual IEchoXSettings* getSettings() { return nullptr; }
    IEchoSettingsTreeNode* mParent = nullptr;
    QList<QSharedPointer<IEchoSettingsTreeNode>> mChildren;
};

class EchoSettingsTreeNode_Settings : public IEchoSettingsTreeNode {
public:
    EchoSettingsTreeNode_Settings(IEchoXSettings* settings)
        : mSettings(settings) {}
    QString getName() override { return mSettings->getName(); }
    IEchoXSettings* getSettings() override { return mSettings; }
    IEchoXSettings* mSettings = nullptr;
};

class EchoSettingsTreeNode_Categroy : public IEchoSettingsTreeNode {
public:
    EchoSettingsTreeNode_Categroy(const QString& inName)
        : mCategoryName(inName) {}
    QString getName() override { return mCategoryName; }
    void addSettings(IEchoXSettings* settings) {
        QSharedPointer<EchoSettingsTreeNode_Settings> settingsNode(new EchoSettingsTreeNode_Settings(settings));
        settingsNode->mParent = this;
        mChildren << settingsNode;
    }
    QString mCategoryName;
};

class QEchoXSettingsModel : public QAbstractItemModel {
    Q_OBJECT
        enum Roles {
        name = 0,
    };
public:
    QEchoXSettingsModel(QObject* parent = 0);
    ~QEchoXSettingsModel() {}
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    void refresh();
    Q_INVOKABLE void notifyCurrentSettingChanged(QModelIndex index);
Q_SIGNALS:
    void settingsChanged();
    void settingsSelected(IEchoXSettings*);
private:
    QList<QSharedPointer<EchoSettingsTreeNode_Categroy>> mCategoryList;
};

#endif // QEchoXSettingsModel_h__
