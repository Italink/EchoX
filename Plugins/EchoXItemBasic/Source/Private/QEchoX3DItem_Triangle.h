#ifndef QEchoX3DItem_Triangle_h__
#define QEchoX3DItem_Triangle_h__

#include "Project/IEchoX3DItem.h"
#include "QtConcurrent/qtconcurrentrun.h"
#include "Render/Component/QSplineRenderComponent.h"
#include "Render/RenderGraph/PassBuilder/QMeshPassBuilder.h"

class QEchoX3DItemRenderer_Triangle : public IEchoX3DItemRenderer {
private:
	QSplineRenderComponent mSplineComp;
	QSharedPointer<QMeshPassBuilder> mMeshPass{ new QMeshPassBuilder };
public:
	QEchoX3DItemRenderer_Triangle()
	{
		QList<QSplinePoint> points;

		int numOfPoint = 10;
		int scaleFactor = 10;

		for (int i = 0; i < numOfPoint; i++) {
			QSplinePoint point;
			point.mPoint.setX((i - numOfPoint / 2) * scaleFactor);
			point.mPoint.setY(i % 2 ? scaleFactor : -scaleFactor);
			points << point;
		}

		mSplineComp.setPoints(points);

		addComponent(&mSplineComp);
	}
protected:
	void setupGraph(QRenderGraphBuilder& graphBuilder) override {
		QMeshPassBuilder::Output meshOut
			= graphBuilder.addPassBuilder("MeshPass", mMeshPass);
		graphBuilder.addPass([this, rt = graphBuilder.getMainRenderTarget()](QRhiCommandBuffer* cmdBuffer) {
			cmdBuffer->beginPass(rt, QColor::fromRgbF(0.0f, 0.0f, 0.0f, 0.0f), { 1.0f, 0 });
			cmdBuffer->endPass();
		});
	}
};

class QEchoX3DItem_Triangle : IEchoX3DItem {
	Q_OBJECT
public: 
	Q_INVOKABLE QEchoX3DItem_Triangle() {}
protected: 
	IEchoX3DItemRenderer* createRenderer() override {
		return new QEchoX3DItemRenderer_Triangle();
	}
};

#endif // QEchoX3DItem_Triangle_h__
