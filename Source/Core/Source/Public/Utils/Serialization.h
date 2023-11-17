#ifndef Serialization_h__
#define Serialization_h__

#include "EchoXCoreAPI.h"
#include <QCborArray>
#include <QCborMap>
#include <QDir>

class ECHOXCORE_API Serialization {
public:
	static void registerBuiltinType();
	static QCborValue toCborValue(const QVariant& var);
	static QCborMap toCbor(QObject* object);
	static void fromCbor(QObject* object, QCborMap cbor);
};

#endif // Serialization_h__
