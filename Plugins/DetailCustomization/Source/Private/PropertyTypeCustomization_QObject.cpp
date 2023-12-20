#include "PropertyTypeCustomization_QObject.h"
#include <QObject>
#include <QHBoxLayout>
#include "Utils/MathUtils.h"
#include "Object/QEngineObjectManager.h"

void PropertyTypeCustomization_QObject::customizeHeader(QPropertyHandle* PropertyHandle, IHeaderRowBuilder* Builder) {
	if (PropertyHandle->hasMetaData("Instanced")) {
		QVariant var = PropertyHandle->getValue();
		QMetaType metaType = PropertyHandle->getType();
		QRegularExpression reg("QSharedPointer\\<(.+)\\>");
		QRegularExpressionMatch match = reg.match(metaType.name());
		QStringList matchTexts = match.capturedTexts();
		QMetaType innerMetaType;
		const QMetaObject* imgMetaObject = nullptr;
		if (!matchTexts.isEmpty()) {
			innerMetaType =  QMetaType::fromName((matchTexts.back()).toLocal8Bit());
			imgMetaObject = mMetaObject = innerMetaType.metaObject();
			mObjectPtr = *(QObject**)var.data();
			if (mObjectPtr) {
				mMetaObject = mObjectPtr->metaObject();
			}
		}		
		QComboBox* derivedMetaObjectSelector = new QComboBox;
		derivedMetaObjectSelector->addItem("None");
		if (mMetaObject) {
			QList<const QMetaObject*> derivedClasses = QEngineObjectManager::Get().getDerivedMetaObjects(imgMetaObject);
			for (auto derivedClass : derivedClasses) {
				derivedMetaObjectSelector->addItem(derivedClass->className());
			}
		}
		derivedMetaObjectSelector->setCurrentText(mObjectPtr == nullptr ? "None" : mObjectPtr->metaObject()->className());
		QObject::connect(derivedMetaObjectSelector, &QComboBox::currentTextChanged, derivedMetaObjectSelector, [metaType, innerMetaType, PropertyHandle](const QString& text) {
			const QMetaObject* metaObject = QEngineObjectManager::Get().getMetaObjectByName(text);
			QObject* newObject = nullptr;
			if (metaObject) {
				newObject = metaObject->newInstance();
				if (!newObject) {
					QMetaType metaType = metaObject->metaType();
					newObject = (QObject*)metaType.create();
				}
			}
			QVariant sharedPtr(metaType);
			memcpy(sharedPtr.data(), &newObject, sizeof(newObject));
			QtSharedPointer::ExternalRefCountData* data = ExternalRefCountWithMetaType::create(innerMetaType, newObject);
			memcpy((char*)sharedPtr.data() + sizeof(newObject), &data, sizeof(data));
			PropertyHandle->setValue(sharedPtr);
			Q_EMIT PropertyHandle->asRequestRebuildRow();
		});
		
		QWidget* valueContent = new QWidget;
		valueContent->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
		QHBoxLayout* valueContentLayout = new QHBoxLayout(valueContent);
		valueContentLayout->setAlignment(Qt::AlignLeft);
		valueContentLayout->setContentsMargins(10, 2, 10, 2);
		valueContentLayout->setSpacing(2);
		valueContentLayout->addWidget(derivedMetaObjectSelector);
		PropertyHandle->generateAttachButtonWidget(valueContentLayout);
		Builder->setNameValueWidget(PropertyHandle->generateNameWidget(), valueContent);
	}
	else {
		Builder->setNameValueWidget(PropertyHandle->generateNameWidget(), PropertyHandle->generateValueWidget());
	}
}

void PropertyTypeCustomization_QObject::customizeChildren(QPropertyHandle* PropertyHandle, IDetailLayoutBuilder* Builder) {
	if (mMetaObject && mObjectPtr) {
		IDetailLayoutBuilder::ObjectContext Context;
		Context.MetaObject = mMetaObject;
		Context.ObjectPtr = mObjectPtr;
		Context.OwnerObject = mObjectPtr;
		Builder->addObject(Context);
	}
}
