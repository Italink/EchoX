#include "CustomPlugin.h"
#include "CustomSettings.h"

void CustomPlugin::startup() {
	QCustomSettings::Register();
}

void CustomPlugin::shutdown() {
	QCustomSettings::Unregister();
}

QENGINE_IMPLEMENT_PLUGIN(CustomPlugin, CustomPlugin)