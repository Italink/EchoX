//#include "QEchoXSettingsOutliner.h"
//#include <QPainter>
//#include "qcoreevent.h"
//#include "Settings/IEchoXSettings.h"
//#include "QEngineEditorStyleManager.h"
//
//QEchoXSettingsOutliner::QEchoXSettingsOutliner() {
//	setHeaderHidden(true);
//	setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
//	setIndentation(16);
//	QFont font = QEngineEditorStyleManager::Instance()->getFont();
//	font.setPointSize(14);
//	setFont(font);
//	setItemsExpandable(false);
//	connect(this, &QTreeWidget::currentItemChanged, this, &QEchoXSettingsOutliner::onCurrentItemChanged);
//	connect(&QSettingsManager::Get(), &QSettingsManager::asSettingsChanged, this, &QEchoXSettingsOutliner::forceRefresh);
//}
//
//void QEchoXSettingsOutliner::drawRow(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const
//{
//	bool hovered = false;
//	bool seleted = false;
//	bool hasChildren = false;
//	bool isExpanded = false;
//	int level = 0;
//	QTreeWidgetItem* item = itemFromIndex(index);
//	QModelIndex parent = index.parent();
//	while (parent.isValid()) {
//		level++;
//		parent = parent.parent();
//	}
//	QRect branchRect(options.rect.x() + level * indentation(), options.rect.y(), indentation(), options.rect.height());
//	QPoint mousePos = mapFromGlobal(QCursor::pos());
//	seleted = item->isSelected();
//	hovered = options.rect.contains(mousePos);
//	hasChildren = item->childCount() > 0;
//	isExpanded = item->isExpanded();
//	painter->save();
//	painter->fillRect(options.rect.adjusted(level * indentation(), 0, 0, 0), QEngineEditorStyleManager::Instance()->getCategoryColor());
//	painter->setPen(Qt::NoPen);
//
//	if (level > 0) {
//		if (hovered) {
//			painter->setBrush(QEngineEditorStyleManager::Instance()->getHoveredColor());
//			painter->drawRect(options.rect);
//		}
//		if (seleted) {
//			painter->setBrush(QEngineEditorStyleManager::Instance()->getSelectedColor());
//			painter->drawRect(options.rect);
//		}
//	}
//	QStyleOptionViewItem opt = options;
//	opt.rect.moveLeft((level + 1) * indentation());
//	if (level > 0) {
//		opt.font.setPointSize(9);
//	}
//	painter->restore();
//	itemDelegateForIndex(index)->paint(painter, opt, index);
//}
//
//void QEchoXSettingsOutliner::forceRefresh() {
//	clear();
//	const QList<IEchoXSettings*>& allSettings = QSettingsManager::Get().getAllSettings();
//	QMap<QString, QTreeWidgetItem*> categoryMap;
//	for (auto& settings : allSettings) {
//		QString category = settings->category();
//		if (!categoryMap.contains(category)) {
//			QTreeWidgetItem* categoryItem = new QTreeWidgetItem({ category });
//			addTopLevelItem(categoryItem);
//			categoryMap[category] = categoryItem;
//		}
//		QTreeWidgetItem* settingsItem = new QTreeWidgetItem({ settings->getName() });
//		settingsItem->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue(settings));
//		categoryMap[category]->addChild(settingsItem);
//	}
//	expandAll();
//}
//
//void QEchoXSettingsOutliner::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous) 
//{
//	if (current) {
//		QVariant var = current->data(0, Qt::ItemDataRole::UserRole);
//		IEchoXSettings* settings = var.value<IEchoXSettings*>();
//		Q_EMIT asSettingsSelected(settings);
//	}
//}
