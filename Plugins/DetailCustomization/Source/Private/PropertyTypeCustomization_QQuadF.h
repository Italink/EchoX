#ifndef PropertyTypeCustomization_QQuadF_h__
#define PropertyTypeCustomization_QQuadF_h__

#include "DetailView/IPropertyTypeCustomization.h"
#include "Window3D/QQuadF.h"

class PropertyTypeCustomization_QQuadF :public IPropertyTypeCustomization {
public:
	void customizeHeader(QPropertyHandle* PropertyHandle, IHeaderRowBuilder* Builder) override;
	void customizeChildren(QPropertyHandle* PropertyHandle, IDetailLayoutBuilder* Builder) override;
};

#endif // PropertyTypeCustomization_QQuadF_h__
