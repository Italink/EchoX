#include "QEchoXSettingsPage.h"
#include "Settings/QSettingsManager.h"
#include "Settings/IEchoXSettings.h"
#include <QMetaClassInfo>

QEchoXSettingsPage::QEchoXSettingsPage()
	: mSettingsOutliner(new QTreeWidget())
	, mSettingsView(new QDetailView)
{
	QHBoxLayout* hLayout = new QHBoxLayout(this);
	hLayout->addWidget(mSettingsOutliner,3);
	hLayout->addWidget(mSettingsView,7);
	mSettingsOutliner->setHeaderHidden(true);
	connect(&QSettingsManager::Get(), &QSettingsManager::asSettingsChanged, this, &QEchoXSettingsPage::refreshSettings);
	connect(mSettingsOutliner, &QTreeWidget::currentItemChanged, this, &QEchoXSettingsPage::onCurrentItemChanged);
	refreshSettings();
}

void QEchoXSettingsPage::refreshSettings()
{
	mSettingsOutliner->clear();
	const QList<IEchoXSettings*>& allSettings = QSettingsManager::Get().getAllSettings();
	QMap<QString, QTreeWidgetItem*> categoryMap;
	for (auto& settings : allSettings) {
		QString category = settings->category();
		if (!categoryMap.contains(category)) {
			QTreeWidgetItem* categoryItem = new QTreeWidgetItem({ category });
			mSettingsOutliner->addTopLevelItem(categoryItem);
			categoryMap[category] = categoryItem;
		}
		QTreeWidgetItem* settingsItem = new QTreeWidgetItem({ settings->metaObject()->classInfo(settings->metaObject()->indexOfClassInfo("ClassName")).value()});
		settingsItem->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue(settings));
		categoryMap[category]->addChild(settingsItem);
	}
	mSettingsOutliner->expandAll();
}

void QEchoXSettingsPage::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (current) {
		QVariant var = current->data(0, Qt::ItemDataRole::UserRole);
		IEchoXSettings* settings = var.value<IEchoXSettings*>();
		if (settings) {
			mSettingsView->setObject(settings);
			return;
		}
	}
	mSettingsView->setObject(nullptr);
}

