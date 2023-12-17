#include "QEchoXSettingsPage.h"
#include "Settings/QSettingsManager.h"
#include "Settings/IEchoXSettings.h"
#include <QMetaClassInfo>

QEchoXSettingsPage::QEchoXSettingsPage()
	: mSplitter(new QSplitter)
	, mSettingsOutliner(new QTreeWidget())
	, mSettingsViewBox(new QWidget)
{
	QHBoxLayout* hLayout = new QHBoxLayout(this);
	hLayout->addWidget(mSplitter);
	mSplitter->addWidget(mSettingsOutliner);
	mSplitter->addWidget(mSettingsViewBox);
	mSplitter->setSizes({ 300,800 });
	QHBoxLayout* viewLayout = new QHBoxLayout(mSettingsViewBox);
	mSettingsOutliner->setHeaderHidden(true);
	mSettingsOutliner->setIndentation(8);
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
		QTreeWidgetItem* settingsItem = new QTreeWidgetItem({ settings->getName() });
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
			while (QLayoutItem* item = mSettingsViewBox->layout()->takeAt(0)){
				if (QWidget* widget = item->widget())
					widget->deleteLater();
				delete item;
			}
			QWidget* view = settings->createWidget();
			connect(view, &QObject::destroyed, settings, &IEchoXSettings::save);
			mSettingsViewBox->layout()->addWidget(view);
			return;
		}
	}
}

