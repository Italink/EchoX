#include "Serialization.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QJsonArray>
#include <QSequentialIterable>
#include <QAssociativeIterable>
#include "qvectornd.h"

QJsonValue Serialization::toJsonValue(const QVariant& var)
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
			array << toJsonValue(iterable.at(index));
		}
		return array;
	}
	else if (QMetaType::canConvert(metaType, QMetaType::fromType<QVariantMap>())) {
		QJsonObject object;
		QAssociativeIterable iterable = var.value<QAssociativeIterable>();
		for (auto iter = iterable.begin(); iter != iterable.end(); ++iter) {
			object.insert(iter.key().toString(), toJsonValue(iter.value()));
		}
		return object;
	}
	else if (metaType.flags() & QMetaType::IsEnumeration) {
		return QJsonValue(var.toInt());
	}
	else if (metaType == QMetaType::fromType<QVector2D>()) {
		QVector2D vec = var.value<QVector2D>();
		QJsonArray array;
		array << vec.x();
		array << vec.y();
		return array;
	}
	else if (metaType == QMetaType::fromType<QVector3D>()) {
		QVector3D vec = var.value<QVector3D>();
		QJsonArray array;
		array << vec.x();
		array << vec.y();
		array << vec.z();
		return array;
	}
	else if (metaType == QMetaType::fromType<QVector4D>()) {
		QVector4D vec = var.value<QVector4D>();
		QJsonArray array;
		array << vec.x();
		array << vec.y();
		array << vec.z();
		array << vec.w();
		return array;
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
			object.insert("ClassName", metaObject->className());
			for (int i = 0; i < metaObject->propertyCount(); i++) {
				QMetaProperty prop = metaObject->property(i);
				QJsonValue value;
				if (metaObject->inherits(&QObject::staticMetaObject)) {
					QObject* objectPtr = var.value<QObject*>();
					if (objectPtr) {
						value = toJsonValue(prop.read((QObject*)objectPtr));
					}
				}
				else {
					const void* ptr = var.data();
					if (metaType.flags().testFlag(QMetaType::IsPointer))
						ptr = *(void**)var.data();
					value = toJsonValue(prop.readOnGadget(ptr));
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

QJsonObject Serialization::toJson(QObject* object)
{
	return toJsonValue(QVariant::fromValue(object)).toObject();
}

QVariant Serialization::fromJsonValue(const QJsonValue& value, QMetaType type)
{
	return QVariant();
}

void Serialization::fromJson(QObject* object, QJsonObject json)
{
	if (object) {
		const QMetaObject* metaObject = object->metaObject();
		for (int i = 0; i < metaObject->propertyCount(); i++) {
			QMetaProperty prop = metaObject->property(i);
			if (json.contains(prop.name())) {
				QMetaType propMetaType = prop.metaType();
				bool bIsPointer = propMetaType.flags().testFlag(QMetaType::IsPointer);
				QRegularExpression reg("(QSharedPointer|std::shared_ptr|shared_ptr)\\<(.+)\\>");
				QRegularExpressionMatch match = reg.match(propMetaType.name());
				QStringList matchTexts = match.capturedTexts();
				if (!matchTexts.isEmpty()) {
					bIsPointer = true;
				}
			}
		}
	}
}

