#include "QEchoXPluginsSettings.h"
#include "QEchoXApplication.h"

QEchoXPluginsSettings::QEchoXPluginsSettings()
{
}

QQuickItem* QEchoXPluginsSettings::createView(QQmlEngine* engine)
{
	QQmlComponent component = QQmlComponent(engine,QUrl("qrc:///Resources/Qml/SettingsView/PluginView.qml"));
	return qobject_cast<QQuickItem*>(component.create());
}