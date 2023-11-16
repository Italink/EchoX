#include "Serialization.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QJsonArray>
#include <QSequentialIterable>
#include <QAssociativeIterable>
#include "qvectornd.h"

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
	registerType<QVector2D, QJsonValue>([](const QVector2D& var)->QJsonValue {
		QJsonArray array;
		array << var.x();
		array << var.y();
		return array;
	});

	registerType<QJsonValue, QVector2D>([](const QJsonValue& var)->QVector2D {
		QJsonArray array = var.toArray();
		QVector2D vec;
		if (array.size() == 2) {
			vec.setX(array[0].toDouble());
			vec.setY(array[1].toDouble());
		}
		return vec;
	});

	registerType<QVector3D, QJsonValue>([](const QVector3D& var)->QJsonValue {
		QJsonArray array;
		array << var.x();
		array << var.y();
		array << var.z();
		return array;
	});

	registerType<QJsonValue, QVector3D>([](const QJsonValue& var)->QVector3D {
		QJsonArray array = var.toArray();
		QVector3D vec;
		if (array.size() == 3) {
			vec.setX(array[0].toDouble());
			vec.setY(array[1].toDouble());
			vec.setZ(array[2].toDouble());
		}
		return vec;
	});

	registerType<QVector4D, QJsonValue>([](const QVector4D& var)->QJsonValue {
		QJsonArray array;
		array << var.x();
		array << var.y();
		array << var.z();
		array << var.w();
		return array;
	});

	registerType<QJsonValue, QVector4D>([](const QJsonValue& var)->QVector4D {
		QJsonArray array = var.toArray();
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

QJsonValue Serialization::toJsonValue(const QVariant& var, const Context& context)
{
	if (var.isNull()) {
		return QJsonValue();
	}
	QMetaType metaType = var.metaType();
	if (QMetaType::canConvert(metaType, QMetaType::fromType<QJsonValue>())) {
		return var.toJsonValue();
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QString>())) {
		return QJsonValue(var.toString());
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QVariantList>())) {
		QJsonArray array;
		QSequentialIterable iterable = var.value<QSequentialIterable>();
		for (int index = 0; index < iterable.size(); index++) {
			array << toJsonValue(iterable.at(index), context);
		}
		return array;
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QVariantMap>())) {
		QJsonObject object;
		QAssociativeIterable iterable = var.value<QAssociativeIterable>();
		for (auto iter = iterable.begin(); iter != iterable.end(); ++iter) {
			object.insert(iter.key().toString(), toJsonValue(iter.value(), context));
		}
		return object;
	}
	else if (metaType.flags() & QMetaType::IsEnumeration) {
		return QJsonValue(var.toInt());
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
			if (metaObject->inherits(&QObject::staticMetaObject)) {
				QObject* objectPtr = var.value<QObject*>();
				if (objectPtr) {
					metaObject = objectPtr->metaObject();
				}
			}
		}
		if (metaObject) {
			QJsonObject object;
			for (int i = 0; i < metaObject->propertyCount(); i++) {
				QMetaProperty prop = metaObject->property(i);
				QJsonValue value;
				if (metaObject->inherits(&QObject::staticMetaObject)) {
					QObject* objectPtr = var.value<QObject*>();
					if (objectPtr) {
						value = toJsonValue(prop.read((QObject*)objectPtr), context);
					}
				}
				else {
					const void* ptr = var.data();
					if (metaType.flags().testFlag(QMetaType::IsPointer))
						ptr = *(void**)var.data();
					value = toJsonValue(prop.readOnGadget(ptr), context);
				}
				if (!value.isNull()) {
					object.insert(prop.name(), value);
				}
			}
			return object;
		}
	}
	return QJsonValue("Invalid Type");
}

QJsonObject Serialization::toJson(QObject* object, const Context& context)
{
	return toJsonValue(QVariant::fromValue(object), context).toObject();
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

QVariant fromJsonValue(const QJsonValue& value, QMetaType metaType) {
	if (QMetaType::canConvert(QMetaType::fromType<QJsonValue>(), metaType)) {
		QVariant var(metaType);
		QMetaType::convert(QMetaType::fromType<QJsonValue>(), &value, metaType, var.data());
		return var;
	}
	else if (value.isString() && QMetaType::canConvert(QMetaType::fromType<QString>(), metaType)) {
		QString string = value.toString();
		QVariant var(metaType);
		QMetaType::convert(QMetaType::fromType<QString>(), &string, metaType, var.data());
		return var;
	}
	else if (metaType.flags() & QMetaType::IsEnumeration) {
		return value.toInt();
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QVariantList>())) {
		QJsonArray array = value.toArray();
		QVariant varList(metaType);
		QSequentialIterable iterable = varList.value<QSequentialIterable>();
		void* containterPtr = const_cast<void*>(iterable.constIterable());
		auto metaContainer = iterable.metaContainer();
		QMetaType valueMetaType = iterable.valueMetaType();
		QtPrivate::QVariantTypeCoercer coercer;
		for (auto item : array) {
			QVariant var = fromJsonValue(item, valueMetaType);
			const void* dataPtr = coercer.coerce(var, var.metaType());
			metaContainer.addValueAtEnd(containterPtr, dataPtr);
		}
		return varList;
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QVariantMap>())) {
		QJsonObject object = value.toObject();
		QVariant varMap(metaType);
		QAssociativeIterable iterable = varMap.value<QAssociativeIterable>();
		QMetaAssociation metaContainer = iterable.metaContainer();
		QtPrivate::QVariantTypeCoercer keyCoercer;
		QtPrivate::QVariantTypeCoercer mappedCoercer;
		for (auto it = object.begin(); it != object.end(); ++it) {
			void* containterPtr = const_cast<void*>(iterable.constIterable());
			QVariant var = fromJsonValue(it.value(), metaContainer.mappedMetaType());
			const void* dataPtr = mappedCoercer.coerce(var, var.metaType());
			metaContainer.setMappedAtKey(containterPtr, keyCoercer.coerce(it.key(), metaContainer.keyMetaType()), dataPtr);
		}
		return varMap;
	}
	else {
		QJsonObject object = value.toObject();
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
			if (metaObject && metaObject->inherits(&QObject::staticMetaObject)) {
				QObject* obj = metaObject->newInstance();
				if (obj)
					newObject = QVariant::fromValue(obj);
				else {
					QMetaType innerMetaType = QMetaType::fromName(QString(metaType.name()).remove("*").toLocal8Bit());
					if (innerMetaType.isValid()) {
						void* ptr = innerMetaType.create();
						QVariant var(metaType, ptr);
						memcpy(var.data(), &ptr, sizeof(ptr));
						return var;
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
				if (object.contains(prop.name())) {
					QJsonValue value = object.value(prop.name());
					QVariant var = fromJsonValue(value, prop.metaType());
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
	}
	return QVariant();
}

void Serialization::fromJson(QObject* object, QJsonObject json)
{
	if (object) {
		const QMetaObject* metaObject = object->metaObject();
		for (int i = 0; i < metaObject->propertyCount(); i++) {
			QMetaProperty prop = metaObject->property(i);
			QString name = prop.name();
			if (json.contains(name)) {
				QJsonValue value = json.value(prop.name());
				QVariant var = fromJsonValue(value, prop.metaType());
				qDebug() << var;
				if (var.isValid()) {
					prop.write(object, var);
				}
			}
		}
	}
}

