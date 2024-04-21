#ifndef IEchoX3DItem_h__
#define IEchoX3DItem_h__

#include "IEchoXVisualItem.h"
#include "Render/IRenderer.h"

class ECHOXCORE_API IEchoX3DItemRenderer : public IRenderer {
	Q_OBJECT
public:
	Q_PROPERTY(QMatrix4x4 Transform READ getTransform WRITE setTransform)
public:
	QMatrix4x4 getTransform() const { return mTransform; }
	void setTransform(QMatrix4x4 val) { mTransform = val; }
private:
	QMatrix4x4 mTransform;
};

class ECHOXCORE_API IEchoX3DItem : public IEchoXVisualItem{
	Q_OBJECT
	Q_PROPERTY(IEchoX3DItemRenderer* Renderer READ getRenderer)
public:
	IEchoX3DItem();
	IEchoX3DItemRenderer* getRenderer() { return mRenderer; }
protected:
	virtual IEchoX3DItemRenderer* createRenderer() = 0;
	virtual void activate() override;
	virtual void deactivate() override;
private:
	IEchoX3DItemRenderer* mRenderer = nullptr;
	QWidget* mViewport = nullptr;
};

#define ECHOX_DECLARE_3DITEM(RendererType, ItemName) \
	class ItemName : IEchoX3DItem { \
		Q_OBJECT \
	protected: \
		IEchoX3DItemRenderer* createRenderer() override { return new RendererType(); } \
	};

#endif // IEchoX3DItem_h__
