#ifndef IEchoXSettings_h__
#define IEchoXSettings_h__

#include <QObject>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include "QSettingsManager.h"
#include "EchoXCoreAPI.h"

#define ECHOX_CONF_VAR(Type, Name)\
public: \
    Q_PROPERTY(Type Name READ get##Name WRITE set##Name NOTIFY as##Name##Changed STORED true) \
    Q_SIGNAL void as##Name##Changed(Type); \
    Type get##Name(){ return m##Name; } \
    void set##Name(Type var){ \
        if(m##Name != var) { \
            m##Name = var; \
            Q_EMIT as##Name##Changed(var); \
        } \
    } \
private: \
    Type m##Name

#define ECHOX_DECLARE_SETTINGS(SettingsType,SettingName) \
        Q_CLASSINFO("ClassName",#SettingName) \
        static SettingsType* Get() {  \
            return QSettingsManager::Get().GetSettings<SettingsType>(); \
        }  \
        static void Register() {  \
            QSettingsManager::Get().registerSettings<SettingsType>(); \
        }  \
        static void Unregister() {  \
            QSettingsManager::Get().unregisterSettings<SettingsType>(); \
        }  \

class ECHOXCORE_API IEchoXSettings: public QObject {
public:
    virtual QQuickItem* createView(QQmlEngine* engine);
    virtual QString category() const { return "QEchoX"; }
    virtual bool visible() const { return true; }

    QString getName() const;
    QFile getSettingsFile() const;
    bool save();
    bool reload();
};

#endif // IEchoXSettings_h__
