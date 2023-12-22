#include "QEchoXComponentOutliner.h"
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

QEchoXComponentOutliner::QEchoXComponentOutliner() {
	setHeaderHidden(true);
	setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	setIndentation(16);
	QFont font = QEngineEditorStyleManager::Instance()->getFont();
	font.setPointSize(9);
	setFont(font);
	connect(&QProjectsManager::Get(), &QProjectsManager::asCurrentProjectChanged, this, &QEchoXComponentOutliner::refreshOutliner);
	connect(&QProjectsManager::Get(), &QProjectsManager::asCurrentProjectComponentChanged, this, &QEchoXComponentOutliner::refreshOutliner);
	connect(this, &QTreeWidget::itemClicked, this, &QEchoXComponentOutliner::onItemClicked);
	refreshOutliner();
	mEyeClose.reset(new QSvgIcon(":/Resources/no_eye.png", "Outliner"));
	mEye.reset(new QSvgIcon(":/Resources/eye.png", "Outliner"));
}

void QEchoXComponentOutliner::refreshOutliner()
{
	clear();
	QEchoXProject* project = QProjectsManager::Get().getCurrentProject();
	if (project) {
		for (IEchoXComponent* item : project->getItems()) {
			QTreeWidgetItem* wItem = new QTreeWidgetItem({ item->metaObject()->className() });
			wItem->setData(0, Qt::ItemDataRole::UserRole, QVariant::fromValue(item));
			wItem->setSizeHint(0, QSize(30, 30));
			addTopLevelItem(wItem);
		}
	}
}

void QEchoXComponentOutliner::drawRow(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const
{
	QTreeWidgetItem* item = itemFromIndex(index);
	IEchoXComponent* component = item->data(0, Qt::ItemDataRole::UserRole).value<IEchoXComponent*>();
	painter->save();
	if (item->isSelected()) {
		painter->setBrush(QEngineEditorStyleManager::Instance()->getHoveredColor());
		painter->drawRect(options.rect);
	}
	const int IconSize = 20;
	QRect eyeGemotry = QRect(options.rect.x() + 5, options.rect.y() + (options.rect.height() - IconSize) / 2, IconSize, IconSize);
	if(!component->isActivated())
		mEyeClose->getIcon().paint(painter, eyeGemotry);
	else {
		QPoint mousePos = mapFromGlobal(QCursor::pos());
		if (eyeGemotry.contains(mousePos)) {
			mEye->getIcon().paint(painter, eyeGemotry);
		}
	}
	QStyleOptionViewItem opt = options;
	opt.rect.moveLeft(IconSize + 10);
	painter->restore();
	itemDelegateForIndex(index)->paint(painter, opt, index);
}

void QEchoXComponentOutliner::onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	IEchoXComponent* component = nullptr;
	if (current) {
		component = current->data(0, Qt::ItemDataRole::UserRole).value<IEchoXComponent*>();
	}
	Q_EMIT asComponentSelected(component);
}

void QEchoXComponentOutliner::onItemClicked(QTreeWidgetItem* item, int column)
{
	IEchoXComponent* component = nullptr;
	if (item) {
		component = item->data(0, Qt::ItemDataRole::UserRole).value<IEchoXComponent*>();
		QPoint localPos = mapFromGlobal(QCursor::pos());
		if (localPos.x() < 40) {
			if (component->isActivated()) {
				component->deactivateInternal();
			}
			else {
				component->activateInternal();
			}
			update();
		}
	}
}
