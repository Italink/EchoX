#ifndef Serialization_h__
#define Serialization_h__

#include "EchoXCoreAPI.h"
#include <QJsonDocument>
#include <QJsonObject>

class ECHOXCORE_API Serialization {
public:
	static QJsonValue toJsonValue(const QVariant& var);
	static QJsonObject toJson(QObject* object);

	static QVariant fromJsonValue(const QJsonValue& value, QMetaType type);
	static void fromJson(QObject* object, QJsonObject json);
};

#endif // Serialization_h__
