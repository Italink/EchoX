#include "QEchoXComponentSelector.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include <QScreen>
#include "qcoreevent.h"
#include "QEngineEditorStyleManager.h"
#include "Project/QProjectsManager.h"
#include "Project/Widget/IEchoXWidgetComponent.h"
#include "Utils/LoggingCategory.h"
#include "QEchoXStyleSettings.h"

class QEchoXDropWidget :public QWidget {
public:
	QEchoXDropWidget() {
		QRect fullscreens;
		for (auto screen : qApp->screens()) {
			QRect localRect = screen->geometry();
			localRect.setSize(localRect.size() * screen->devicePixelRatio());
			fullscreens |= localRect;
		}
		this->setGeometry(fullscreens);
		this->setAcceptDrops(true);
		this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
		this->setAttribute(Qt::WA_TranslucentBackground);
	}
private:
	void dragEnterEvent(QDragEnterEvent* e) {
		e->acceptProposedAction();
	}
	void dropEvent(QDropEvent* e) override {
		if (e->buttons() & Qt::RightButton) {
			e->ignore();
		}
		else {
			e->accept();
		}
	}
	void dragMoveEvent(QDragMoveEvent* e) override {
		e->accept();
	}
	void paintEvent(QPaintEvent* event) override {
		QPainter painter(this);
		painter.fillRect(rect(), QColor(0, 0, 0, 50));
	}
};

QEchoXComponentSelector::QEchoXComponentSelector() {
	setHeaderHidden(true);
	setDragEnabled(true);
	setDropIndicatorShown(false);
	setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
	setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	setIndentation(16);
	QFont font = QEngineEditorStyleManager::Instance()->getFont();
	font.setPointSize(14);
	setFont(font);
	connect(&QProjectsManager::Get(), &QProjectsManager::asComponentTypeInfoChanged, this, &QEchoXComponentSelector::refreshComponentList);
	refreshComponentList();
}

void QEchoXComponentSelector::drawRow(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const
{
	bool hovered = false;
	bool seleted = false;
	bool hasChildren = false;
	bool isExpanded = false;
	int level = 0;
	QTreeWidgetItem* item = itemFromIndex(index);
	QModelIndex parent = index.parent();
	while (parent.isValid()) {
		level++;
		parent = parent.parent();
	}
	QRect branchRect(options.rect.x() + level * indentation(), options.rect.y(), indentation(), options.rect.height());
	QPoint mousePos = mapFromGlobal(QCursor::pos());
	seleted = item->isSelected();
	hovered = options.rect.contains(mousePos);
	hasChildren = item->childCount() > 0;
	isExpanded = item->isExpanded();
	painter->save();
	painter->fillRect(options.rect.adjusted(level * indentation(), 0, 0, 0), QEngineEditorStyleManager::Instance()->getCategoryColor());
	painter->setPen(Qt::NoPen);
	if (level == 0) {
		painter->setBrush(QEchoXStyleSettings::Get()->getThemeColor());
		painter->drawRect(options.rect);
	}
	else {
		if (hovered) {
			painter->setBrush(QEngineEditorStyleManager::Instance()->getHoveredColor());
			painter->drawRect(options.rect);
		}
	}
	QStyleOptionViewItem opt = options;
	opt.rect.moveLeft((level + 1) * indentation());
	if (level > 0) {
		opt.font.setPointSize(9);
	}
	painter->restore();
	itemDelegateForIndex(index)->paint(painter, opt, index);
}

void QEchoXComponentSelector::refreshComponentList()
{
	clear();
	const auto& allComponentTypeInfos = QProjectsManager::Get().getComponentTypeInfos();
	QMap<QString, QTreeWidgetItem*> categoryMap;
	for (auto& componentTypeInfo : allComponentTypeInfos) {
		QString category = componentTypeInfo.category;
		if (!categoryMap.contains(category)) {
			QTreeWidgetItem* categoryItem = new QTreeWidgetItem({ category });
			addTopLevelItem(categoryItem);
			categoryMap[category] = categoryItem;
		}
		QTreeWidgetItem* componentItem = new QTreeWidgetItem({ componentTypeInfo.name });
		componentItem->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue(componentTypeInfo.name));
		categoryMap[category]->addChild(componentItem);
	}
	expandAll();
}

void QEchoXComponentSelector::startDrag(Qt::DropActions supportedActions)
{
	QModelIndexList indexes = selectedIndexes();
	auto isNotDragEnabled = [this](const QModelIndex& index) {
		return !(model()->flags(index) & Qt::ItemIsDragEnabled);
	};
	indexes.removeIf(isNotDragEnabled);
	if (indexes.size() == 1) {
		QString itemTypename = model()->data(indexes[0], Qt::ItemDataRole::UserRole).toString();
		if (itemTypename.isEmpty())
			return;
		if (IEchoXComponent* item = QProjectsManager::Get().createItemByName(itemTypename)) {
			QDrag drag(this);
			drag.setMimeData(model()->mimeData(indexes));
			QPixmap pixmap;
			IEchoXWidgetComponent* widgetItem = qobject_cast<IEchoXWidgetComponent*>(item);
			if (widgetItem) {
				QWidget* widget = widgetItem->widget();
				widget->resize(widgetItem->desiredSize());
				pixmap = QPixmap(widget->size());
				widget->render(&pixmap);
			}
			else {
				pixmap = QPixmap(20, 20);
				pixmap.fill(Qt::red);
			}
			drag.setPixmap(pixmap);
			drag.setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
			QEchoXDropWidget droper;
			droper.show();
			if (drag.exec(Qt::MoveAction) == Qt::MoveAction) {
				if (widgetItem) {
					QRect destRect(0, 0, pixmap.width(), pixmap.height());
					destRect.moveCenter(QCursor::pos());
					widgetItem->setQuad(destRect);
				}
				Q_EMIT asItemDropped(QCursor::pos(), item);
			}
			else {
				item->deleteLater();
			}
		}
		else {
			qCWarning(EchoX) << "Create item faild";
		}
	}
}