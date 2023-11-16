#ifndef Serialization_h__
#define Serialization_h__

#include "EchoXCoreAPI.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QCborArray>
#include <QCborMap>

class ECHOXCORE_API Serialization {
public:
	struct Context{
	};

	static void registerBuiltinType();
	static QCborValue toCborValue(const QVariant& var, const Context& context);
	static QCborMap toCbor(QObject* object, const Context& context);
	static void fromCbor(QObject* object, QCborMap cbor);
};

#endif // Serialization_h__
