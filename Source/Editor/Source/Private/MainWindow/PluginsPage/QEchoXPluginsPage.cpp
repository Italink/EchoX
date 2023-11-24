#include "QEchoXPluginsPage.h"
#include <QMetaClassInfo>
#include "Plugin/QEnginePluginManager.h"

QEchoXPluginsPage::QEchoXPluginsPage()
	: mPluginOutliner(new QListWidget())
{
	QHBoxLayout* hLayout = new QHBoxLayout(this);
	hLayout->addWidget(mPluginOutliner,3);
	connect(&QEnginePluginManager::Get(), &QEnginePluginManager::asPluginChanged, this, &QEchoXPluginsPage::refreshPlugins);
	connect(mPluginOutliner, &QListWidget::currentItemChanged, this, &QEchoXPluginsPage::onCurrentItemChanged);
	refreshPlugins();
}

void QEchoXPluginsPage::refreshPlugins()
{
	mPluginOutliner->clear();
	for (const auto& plugin : QEnginePluginManager::Get().getPluginMap().asKeyValueRange()) {
		QListWidgetItem* pluginItem = new QListWidgetItem({ plugin.first});
		pluginItem->setData( Qt::ItemDataRole::UserRole, QVariant::fromValue(plugin.second));
		mPluginOutliner->addItem(pluginItem);
	}
}

void QEchoXPluginsPage::onCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (current) {
		QVariant var = current->data( Qt::ItemDataRole::UserRole);
		if (!var.isNull()) {
			QEnginePluginHandler pluginInfo = var.value<QEnginePluginHandler>();
		}
	}
}

