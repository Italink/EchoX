#include "PropertyTypeCustomization_QQuadF.h"
#include "Utils/MathUtils.h"

void PropertyTypeCustomization_QQuadF::customizeHeader(QPropertyHandle* PropertyHandle, IHeaderRowBuilder* Builder) {
	Builder->setNameValueWidget(PropertyHandle->generateNameWidget(), PropertyHandle->generateValueWidget());
}

void PropertyTypeCustomization_QQuadF::customizeChildren(QPropertyHandle* PropertyHandle, IDetailLayoutBuilder* Builder) {
	QPropertyHandle* topLeft = QPropertyHandle::FindOrCreate(
		PropertyHandle->parent(),
		QMetaType::fromType<QPointF>(),
		PropertyHandle->getSubPath("TopLeft"),
		[PropertyHandle]() {
			QQuadF quad = PropertyHandle->getValue().value<QQuadF>();
			return quad.topLeft;
		},
		[PropertyHandle](QVariant var) {
			QQuadF quad = PropertyHandle->getValue().value<QQuadF>();
			quad.topLeft = var.value<QPointF>();
			PropertyHandle->setValue(QVariant::fromValue(quad));
		}
	);
	Builder->addProperty(topLeft);

	QPropertyHandle* topRight = QPropertyHandle::FindOrCreate(
		PropertyHandle->parent(),
		QMetaType::fromType<QPointF>(),
		PropertyHandle->getSubPath("TopRight"),
		[PropertyHandle]() {
			QQuadF quad = PropertyHandle->getValue().value<QQuadF>();
			return quad.topRight;
		},
		[PropertyHandle](QVariant var) {
			QQuadF quad = PropertyHandle->getValue().value<QQuadF>();
			quad.topRight = var.value<QPointF>();
			PropertyHandle->setValue(QVariant::fromValue(quad));
		}
	);
	Builder->addProperty(topRight);

	QPropertyHandle* bottomRight = QPropertyHandle::FindOrCreate(
		PropertyHandle->parent(),
		QMetaType::fromType<QPointF>(),
		PropertyHandle->getSubPath("BottomRight"),
		[PropertyHandle]() {
			QQuadF quad = PropertyHandle->getValue().value<QQuadF>();
			return quad.bottomRight;
		},
		[PropertyHandle](QVariant var) {
			QQuadF quad = PropertyHandle->getValue().value<QQuadF>();
			quad.bottomRight = var.value<QPointF>();
			PropertyHandle->setValue(QVariant::fromValue(quad));
		}
	);
	Builder->addProperty(bottomRight);

	QPropertyHandle* bottomLeft = QPropertyHandle::FindOrCreate(
		PropertyHandle->parent(),
		QMetaType::fromType<QPointF>(),
		PropertyHandle->getSubPath("BottomLeft"),
		[PropertyHandle]() {
			QQuadF quad = PropertyHandle->getValue().value<QQuadF>();
			return quad.bottomLeft;
		},
		[PropertyHandle](QVariant var) {
			QQuadF quad = PropertyHandle->getValue().value<QQuadF>();
			quad.bottomLeft = var.value<QPointF>();
			PropertyHandle->setValue(QVariant::fromValue(quad));
		}
	);
	Builder->addProperty(bottomLeft);

}
