#ifndef Serialization_h__
#define Serialization_h__

#include "EchoXCoreAPI.h"
#include <QJsonDocument>
#include <QJsonObject>

class ECHOXCORE_API Serialization {
public:
	struct Context{
	};

	static void registerBuiltinType();

	static QJsonValue toJsonValue(const QVariant& var, const Context& context);
	static QJsonObject toJson(QObject* object, const Context& context);

	static void fromJson(QObject* object, QJsonObject json);
};

#endif // Serialization_h__
