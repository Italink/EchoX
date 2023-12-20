#ifndef PropertyTypeCustomization_QObject_h__
#define PropertyTypeCustomization_QObject_h__

#include "DetailView/IPropertyTypeCustomization.h"
#include <QComboBox>

class PropertyTypeCustomization_QObject :public IPropertyTypeCustomization {
public:
	void customizeHeader(QPropertyHandle* PropertyHandle, IHeaderRowBuilder* Builder) override;
	void customizeChildren(QPropertyHandle* PropertyHandle, IDetailLayoutBuilder* Builder) override;
private:
	const QMetaObject* mMetaObject = nullptr;
	QObject* mObjectPtr = nullptr;
};

#endif // PropertyTypeCustomization_QObject_h__
