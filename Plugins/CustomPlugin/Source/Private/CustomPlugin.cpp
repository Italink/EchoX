#include "CustomPlugin.h"
#include "CustomSettings.h"

CustomPlugin::Info CustomPlugin::info()
{
	CustomPlugin::Info i;
	i.icon = QUrl("qrc:/Resources/delete.png");
	i.name = "CustomPlugin";
	i.author = "italink";
	i.description = "this is a custom plugin";
	i.link = "github.com";
	i.images = { QUrl("qrc:/Resources/delete.png") ,QUrl("qrc:/Resources/delete.png") ,QUrl("qrc:/Resources/delete.png") };
	return i;
}

void CustomPlugin::startup() {
	QCustomSettings::Register();
}

void CustomPlugin::shutdown() {
	QCustomSettings::Unregister();
}

QENGINE_IMPLEMENT_PLUGIN(CustomPlugin, CustomPlugin)