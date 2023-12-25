#include "QEchoXPluginsPage.h"
#include <QMetaClassInfo>
#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QQmlContext>
#include "Plugin/QEnginePluginManager.h"
#include "QEchoXPluginsModel.h"
#include "Settings/QEchoXStyleSettings.h"

QEchoXPluginsPage::QEchoXPluginsPage()
    : mPluginsModel(new QEchoXPluginsModel(this))
{
	QQuickWidget* quick = new QQuickWidget;
	QHBoxLayout* h = new QHBoxLayout(this);
	h->setContentsMargins(10, 10, 10, 10);
	h->addWidget(quick);
	quick->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
	quick->setAttribute(Qt::WA_AlwaysStackOnTop); 
	quick->setClearColor(Qt::transparent);
    quick->rootContext()->setContextProperty("pluginsModel", mPluginsModel);
	quick->rootContext()->setContextProperty("echoxStyle", QEchoXStyleSettings::Get());
	quick->setSource(QUrl("qrc:/Resources/Qml/PluginPage.qml"));

    connect(&QEnginePluginManager::Get(), &QEnginePluginManager::asPluginChanged, mPluginsModel, &QEchoXPluginsModel::refresh);
}