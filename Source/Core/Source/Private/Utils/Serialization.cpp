#include "Serialization.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QSequentialIterable>
#include <QAssociativeIterable>
#include "qvectornd.h"
#include <QImage>
#include <QPixmap>

template<typename From, typename To>
void registerType(std::function<To(const From&)> convertor) {
	QMetaType::registerConverterFunction([convertor](const void* from, void* to)->bool {
			const From* f = static_cast<const From*>(from);
			To* t = static_cast<To*>(to);
			*t = convertor(*f);
			return true;
		}
		, QMetaType::fromType<From>()
		, QMetaType::fromType<To>()
	);
}

void Serialization::registerBuiltinType()
{
	registerType<QImage, QCborValue>([](const QImage& var)->QCborValue {
		QByteArray data;
		QDataStream stream(&data,QDataStream::OpenModeFlag::WriteOnly);
		stream << var;
		return data;
	});

	registerType<QCborValue, QImage>([](const QCborValue& var)->QImage {
		QByteArray data = var.toByteArray();
		QDataStream stream(&data, QDataStream::OpenModeFlag::ReadOnly);
		QImage image;
		stream >> image;
		return image;
	});

	//registerType<QPixmap, QCborValue>([](const QPixmap& var)->QCborValue {
	//	QByteArray data;
	//	QDataStream stream(&data,QDataStream::OpenModeFlag::WriteOnly);
	//	stream << var;
	//	return data;
	//});

	//registerType<QCborValue, QPixmap>([](const QCborValue& var)->QPixmap {
	//	QByteArray data = var.toByteArray();
	//	QDataStream stream(&data, QDataStream::OpenModeFlag::ReadOnly);
	//	QPixmap image;
	//	stream >> image;
	//	return image;
	//});

	registerType<QVector2D, QCborValue>([](const QVector2D& var)->QCborValue {
		QCborArray array;
		array << var.x();
		array << var.y();
		return array;
	});

	registerType<QCborValue, QVector2D>([](const QCborValue& var)->QVector2D {
		QCborArray array = var.toArray();
		QVector2D vec;
		if (array.size() == 2) {
			vec.setX(array[0].toDouble());
			vec.setY(array[1].toDouble());
		}
		return vec;
	});

	registerType<QVector3D, QCborValue>([](const QVector3D& var)->QCborValue {
		QCborArray array;
		array << var.x();
		array << var.y();
		array << var.z();
		return array;
	});

	registerType<QCborValue, QVector3D>([](const QCborValue& var)->QVector3D {
		QCborArray array = var.toArray();
		QVector3D vec;
		if (array.size() == 3) {
			vec.setX(array[0].toDouble());
			vec.setY(array[1].toDouble());
			vec.setZ(array[2].toDouble());
		}
		return vec;
	});

	registerType<QVector4D, QCborValue>([](const QVector4D& var)->QCborValue {
		QCborArray array;
		array << var.x();
		array << var.y();
		array << var.z();
		array << var.w();
		return array;
	});

	registerType<QCborValue, QVector4D>([](const QCborValue& var)->QVector4D {
		QCborArray array = var.toArray();
		QVector4D vec;
		if (array.size() == 4) {
			vec.setX(array[0].toDouble());
			vec.setY(array[1].toDouble());
			vec.setZ(array[2].toDouble());
			vec.setW(array[3].toDouble());
		}
		return vec;
	});


}

QCborValue Serialization::toCborValue(const QVariant& var)
{
	if (var.isNull()) {
		return QCborValue();
	}
	QMetaType metaType = var.metaType();
	if (QMetaType::canConvert(metaType, QMetaType::fromType<QCborValue>())) {
		QCborValue value;
		QMetaType::convert(metaType, var.data(), QMetaType::fromType<QCborValue>(), &value);
		return value;
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QString>())) {
		return QCborValue(var.toString());
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QVariantList>())) {
		QCborArray array;
		QSequentialIterable iterable = var.value<QSequentialIterable>();
		for (int index = 0; index < iterable.size(); index++) {
			array << toCborValue(iterable.at(index));
		}
		return array;
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QVariantMap>())) {
		QCborMap object;
		QAssociativeIterable iterable = var.value<QAssociativeIterable>();
		for (auto iter = iterable.begin(); iter != iterable.end(); ++iter) {
			object.insert(iter.key().toString(), toCborValue(iter.value()));
		}
		return object;
	}
	else if (metaType.flags() & QMetaType::IsEnumeration) {
		return QCborValue(var.toInt());
	}
	else {
		QRegularExpression reg("(QSharedPointer|std::shared_ptr|shared_ptr)\\<(.+)\\>");
		QRegularExpressionMatch match = reg.match(metaType.name(), 0, QRegularExpression::MatchType::PartialPreferCompleteMatch, QRegularExpression::AnchorAtOffsetMatchOption);
		QStringList matchTexts = match.capturedTexts();
		QMetaType innerMetaType;
		const QMetaObject* metaObject = nullptr;
		if (!matchTexts.isEmpty()) {
			QString innerMetaTypeName = matchTexts.back() + "*";
			innerMetaType = QMetaType::fromName(innerMetaTypeName.toLocal8Bit());
			if (!innerMetaType.isValid()) {
				innerMetaType = QMetaType::fromName(matchTexts.back().toLocal8Bit());
			}
			if (!innerMetaType.isValid()) {
				qWarning() << "please use qRegisterMetaType() for " << innerMetaTypeName;
			}
			metaObject = innerMetaType.metaObject();
		}
		else {
			metaObject = metaType.metaObject();
			if (metaObject && metaObject->inherits(&QObject::staticMetaObject)) {
				QObject* objectPtr = var.value<QObject*>();
				if (objectPtr) {
					metaObject = objectPtr->metaObject();
				}
			}
		}
		if (metaObject) {
			QCborMap object;
			for (int i = 0; i < metaObject->propertyCount(); i++) {
				QMetaProperty prop = metaObject->property(i);
				QString propName = prop.name();
				QCborValue value;
				if (metaObject->inherits(&QObject::staticMetaObject)) {
					QObject* objectPtr = var.value<QObject*>();
					if (objectPtr) {
						value = toCborValue(prop.read((QObject*)objectPtr));
					}
				}
				else {
					const void* ptr = var.data();
					if (metaType.flags().testFlag(QMetaType::IsPointer))
						ptr = *(void**)var.data();
					value = toCborValue(prop.readOnGadget(ptr));
				}
				if (!value.isNull()) {
					object.insert(QString(propName), value);
				}
			}
			return object;
		}
		else if (metaType.hasRegisteredDataStreamOperators()) {
			QByteArray data;
			QDataStream stream(&data, QDataStream::OpenModeFlag::WriteOnly);
			stream << var;
			return data;
		}
	}
	return QCborValue("Invalid Type");
}

QCborMap Serialization::toCbor(QObject* object)
{
	return toCborValue(QVariant::fromValue(object)).toMap();
}

struct ExternalRefCountWithMetaType : public QtSharedPointer::ExternalRefCountData {
	typedef ExternalRefCountData Parent;
	QMetaType mMetaType;
	void* mData;

	static void deleter(ExternalRefCountData* self) {
		ExternalRefCountWithMetaType* that =
			static_cast<ExternalRefCountWithMetaType*>(self);
		that->mMetaType.destroy(that->mData);
		Q_UNUSED(that); // MSVC warns if T has a trivial destructor
	}

	static inline ExternalRefCountData* create(QMetaType inMetaType, void* inPtr)
	{
		ExternalRefCountWithMetaType* d = static_cast<ExternalRefCountWithMetaType*>(::operator new(sizeof(ExternalRefCountWithMetaType)));

		// initialize the d-pointer sub-object
		// leave d->data uninitialized
		new (d) Parent(ExternalRefCountWithMetaType::deleter); // can't throw
		d->mData = inPtr;
		d->mMetaType = inMetaType;
		return d;
	}
};

QVariant fromCborValue(const QCborValue& value, QMetaType metaType) {
	if (QMetaType::canConvert(QMetaType::fromType<QCborValue>(), metaType)) {
		QVariant var(metaType);
		QMetaType::convert(QMetaType::fromType<QCborValue>(), &value, metaType, var.data());
		return var;
	}
	else if (value.isString() && QMetaType::canConvert(QMetaType::fromType<QString>(), metaType)) {
		QString string = value.toString();
		QVariant var(metaType);
		QMetaType::convert(QMetaType::fromType<QString>(), &string, metaType, var.data());
		return var;
	}
	else if (metaType.flags() & QMetaType::IsEnumeration) {
		return value.toInteger();
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QVariantList>())) {
		QCborArray array = value.toArray();
		QVariant varList(metaType);
		QSequentialIterable iterable = varList.value<QSequentialIterable>();
		void* containterPtr = const_cast<void*>(iterable.constIterable());
		auto metaContainer = iterable.metaContainer();
		QMetaType valueMetaType = iterable.valueMetaType();
		QtPrivate::QVariantTypeCoercer coercer;
		for (auto item : array) {
			QVariant var = fromCborValue(item, valueMetaType);
			const void* dataPtr = coercer.coerce(var, var.metaType());
			metaContainer.addValueAtEnd(containterPtr, dataPtr);
		}
		return varList;
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QVariantMap>())) {
		QCborMap object = value.toMap();
		QVariant varMap(metaType);
		QAssociativeIterable iterable = varMap.value<QAssociativeIterable>();
		QMetaAssociation metaContainer = iterable.metaContainer();
		QtPrivate::QVariantTypeCoercer keyCoercer;
		QtPrivate::QVariantTypeCoercer mappedCoercer;
		for (auto it = object.begin(); it != object.end(); ++it) {
			void* containterPtr = const_cast<void*>(iterable.constIterable());
			QVariant var = fromCborValue(it.value(), metaContainer.mappedMetaType());
			const void* dataPtr = mappedCoercer.coerce(var, var.metaType());
			metaContainer.setMappedAtKey(containterPtr, keyCoercer.coerce(it.key().toString(), metaContainer.keyMetaType()), dataPtr);
		}
		return varMap;
	}
	else {
		QCborMap object = value.toMap();
		const QMetaObject* metaObject = nullptr;
		QVariant newObject;
		QRegularExpression reg("(QSharedPointer|std::shared_ptr|shared_ptr)\\<(.+)\\>");
		QRegularExpressionMatch match = reg.match(metaType.name());
		QStringList matchTexts = match.capturedTexts();
		bool bIsSharedPointer = false;
		bool bIsPointer = false;
		if (!matchTexts.isEmpty()) {
			QMetaType innerMetaType = QMetaType::fromName((matchTexts.back()).toLocal8Bit());
			metaObject = innerMetaType.metaObject();
			bIsSharedPointer = true;
			bIsPointer = true;
			if (innerMetaType.isValid()) {
				void* ptr = innerMetaType.create();
				QVariant sharedPtr(metaType);
				memcpy(sharedPtr.data(), &ptr, sizeof(ptr));
				QtSharedPointer::ExternalRefCountData* data = ExternalRefCountWithMetaType::create(innerMetaType, ptr);
				memcpy((char*)sharedPtr.data() + sizeof(ptr), &data, sizeof(data));
				newObject = sharedPtr;
			}
		}
		else {
			metaObject = metaType.metaObject();
			bIsPointer = metaType.flags().testFlag(QMetaType::IsPointer);
			if (metaObject) {
				if (metaObject->inherits(&QObject::staticMetaObject)) {
					QObject* obj = metaObject->newInstance();
					if (obj)
						newObject = QVariant::fromValue(obj);
				}
				if(newObject.isNull()){
					QMetaType innerMetaType = QMetaType::fromName(QString(metaType.name()).remove("*").toLocal8Bit());
					if (innerMetaType.isValid()) {
						void* ptr = innerMetaType.create();
						QVariant var(metaType, ptr);
						memcpy(var.data(), &ptr, sizeof(ptr));
						newObject = var;
					}
				}
			}
		}
		if (metaObject) {
			void* objectPtr = newObject.data();
			if (bIsPointer)
				objectPtr = *(void**)objectPtr;
			for (int i = 0; i < metaObject->propertyCount(); i++) {
				QMetaProperty prop = metaObject->property(i);
				if (object.contains(QString(prop.name()))) {
					QCborValue value = object.value(QString(prop.name()));
					QVariant var = fromCborValue(value, prop.metaType());
					if (metaObject->inherits(&QObject::staticMetaObject)) {
						prop.write((QObject*)objectPtr, var);
					}
					else {
						prop.writeOnGadget(objectPtr, var);
					}
				}
			}
			return newObject;
		}
		else if (value.isByteArray() && metaType.hasRegisteredDataStreamOperators()) {
			QByteArray data = value.toByteArray();
			QDataStream stream(&data, QDataStream::OpenModeFlag::ReadOnly);
			QVariant var(metaType);
			stream >> var;
			return var;
		}
	}
	return QVariant();
}

void Serialization::fromCbor(QObject* object, QCborMap cbor)
{
	if (object) {
		const QMetaObject* metaObject = object->metaObject();
		for (int i = 0; i < metaObject->propertyCount(); i++) {
			QMetaProperty prop = metaObject->property(i);
			QString name = prop.name();
			if (cbor.contains(name)) {
				QCborValue value = cbor.value(QString(prop.name()));
				QVariant var = fromCborValue(value, prop.metaType());
				if (var.isValid()) {
					prop.write(object, var);
				}
			}
		}
	}
}

