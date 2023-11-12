#ifndef IEchoXSettings_h__
#define IEchoXSettings_h__

#include <QObject>
#include "EchoXCoreAPI.h"

#define Q_CONF_VAR(Type, Name)\
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

class ECHOXCORE_API IEchoXSettings: public QObject {
public:
	virtual QWidget* createWidget() { return nullptr; }
};

#endif // IEchoXSettings_h__
