#include "QEchoXPluginsPage.h"
#include <QMetaClassInfo>
#include "Plugin/QEnginePluginManager.h"

QEchoXPluginsPage::QEchoXPluginsPage()
{
	setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
	setAttribute(Qt::WA_AlwaysStackOnTop); // the key function
	setClearColor(Qt::transparent);
	setSource(QUrl("qrc:/Qml/PluginPage.qml"));
}