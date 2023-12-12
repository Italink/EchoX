#ifndef QWidgetVFXManager_h__
#define QWidgetVFXManager_h__

#include <QObject>
#include "QWidgetVFXRenderer.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QWidgetVFXManager : public QObject {
	Q_OBJECT
public:
	static QWidgetVFXManager& Get();
	void playWidgetCloseVFX(QWidget* inWidget);
private:
	QWidgetVFXManager();
	void addVFX(IWidgetVFX* inVFX);
private:
	QWidgetVFXRenderer* mRenderer;
	QWidget* mViewport = nullptr;
};

#endif // QWidgetVFXManager_h__
