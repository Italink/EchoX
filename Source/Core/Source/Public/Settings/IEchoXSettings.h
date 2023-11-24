#ifndef IEchoXSettings_h__
#define IEchoXSettings_h__

#include <QObject>
#include "EchoXCoreAPI.h"
#include "DetailView/QDetailView.h"
#include "QSettingsManager.h"

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
        static 	SettingsType* Get() {  \
            return QSettingsManager::Get().GetSettings<SettingsType>(); \
        }  \
		struct __SettingsRegister {\
			__SettingsRegister() {\
				QSettingsManager::Get().registerSettings<SettingsType>();\
			}\
		};\
		inline static __SettingsRegister __register;

class ECHOXCORE_API IEchoXSettings: public QObject {
public:
	virtual QWidget* createWidget() { 
        auto view = new QDetailView();
        view->setObject(this);
        return view; 
    }
    virtual QString category() const { return "QEchoX"; }
};

#endif // IEchoXSettings_h__
