#include "EchoXItemBasic.h"
#include "Project/QEchoXProjectsManager.h"
#include "QEchoX3DItem_Triangle.h"

EchoXItemBasic::Info EchoXItemBasic::info()
{
	EchoXItemBasic::Info i;
	i.icon = QUrl("qrc:/Resources/delete.png");
	i.name = "EchoXItemBasic";
	i.author = "italink";
	i.description = "this is a custom plugin";
	i.link = "github.com";
	return i;
}

void EchoXItemBasic::startup() {
	QEchoXProjectsManager::Get().registerItemType(&QEchoX3DItem_Triangle::staticMetaObject);
}

void EchoXItemBasic::shutdown() {
	QEchoXProjectsManager::Get().unregisterItemType(&QEchoX3DItem_Triangle::staticMetaObject);
}

QENGINE_IMPLEMENT_PLUGIN(EchoXItemBasic, EchoXItemBasic)