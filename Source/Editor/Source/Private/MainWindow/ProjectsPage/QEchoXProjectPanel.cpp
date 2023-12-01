#include "QEchoXProjectPanel.h"
#include <QPushButton>
#include <QPainter>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include <QScreen>
#include "Utils/LoggingCategory.h"

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
		this->setWindowFlag(Qt::FramelessWindowHint);
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
};

void QEchoXProjectItemListWidget::startDrag(Qt::DropActions supportedActions)
{
	QModelIndexList indexes = selectedIndexes();
	auto isNotDragEnabled = [this](const QModelIndex& index) {
		return !(model()->flags(index) & Qt::ItemIsDragEnabled);
		};
	indexes.removeIf(isNotDragEnabled);
	if (indexes.size()  ==  1) {
		QString itemTypename = model()->data(indexes[0], Qt::ItemDataRole::UserRole).toString();
		if (IEchoXItem* item = QProjectsManager::Get().createItemByName(itemTypename)) {
			QDrag drag(this);
			drag.setMimeData(model()->mimeData(indexes));
			QPixmap pixmap(item->desiredSize());
			pixmap.fill(Qt::red);
			drag.setPixmap(pixmap);
			drag.setHotSpot(QPoint(pixmap.width()/2, pixmap.height() / 2));
			QEchoXDropWidget droper;
			droper.show();
			if (drag.exec(Qt::MoveAction) == Qt::MoveAction) {
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

QEchoXProjectPanel::QEchoXProjectPanel()
	: mBtReturn(new QPushButton("Return"))
	, mItemResouceList(new QEchoXProjectItemListWidget)
	, mOutliner(new QListWidget)
	, mDetailView(new QDetailView)
{
	mBtReturn->setIcon(QIcon(":/Resources/return.png"));
	mBtReturn->setMaximumWidth(100);

	mItemResouceList->setSelectionMode(QAbstractItemView::SingleSelection);
	mItemResouceList->setDragEnabled(true);
	//mItemResouceList->setAcceptDrops(true);
	mItemResouceList->setDropIndicatorShown(false);
	mItemResouceList->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(5, 5, 5, 5);
	vLayout->addWidget(mBtReturn, 0, Qt::AlignLeft);

	QSplitter* hSplitter = new QSplitter;
	hSplitter->addWidget(mItemResouceList);
	hSplitter->setSizes({ 150, 600 });

	QSplitter* vSplitter = new QSplitter(Qt::Vertical);
	vSplitter->addWidget(mOutliner);
	vSplitter->addWidget(mDetailView);

	hSplitter->addWidget(vSplitter);

	vLayout->addWidget(hSplitter);
	vSplitter->setSizes({ 250,550 });

	connect(mBtReturn, &QPushButton::clicked, this, &QEchoXProjectPanel::asReturnBack);
	connect(mItemResouceList, &QEchoXProjectItemListWidget::asItemDropped, this, &QEchoXProjectPanel::onItemDropped);
	connect(mOutliner,&QListWidget::currentItemChanged, this, &QEchoXProjectPanel::onCurrentItemChanged);
	connect(&QProjectsManager::Get(), &QProjectsManager::asItemTypesChanged, this, &QEchoXProjectPanel::refreshItemList);
	refreshItemList();
}

void QEchoXProjectPanel::setProject(QEchoXProject* inProject)
{
	if (mProject) 
		mProject->disconnect(this);
	mProject = inProject;
	if (mProject) {
		connect(mProject, &QEchoXProject::asItemsChanged, this, &QEchoXProjectPanel::refreshOutliner);
		refreshOutliner();
	}
}

QEchoXProject* QEchoXProjectPanel::getProject() const
{
	return mProject;
}

void QEchoXProjectPanel::refreshItemList()
{
	mItemResouceList->clear();
	for (const QString& itemType : QProjectsManager::Get().getItemsMap().keys()) {
		QListWidgetItem* resourceItem = new QListWidgetItem(itemType);
		resourceItem->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue(itemType));
		mItemResouceList->addItem(resourceItem);
	}
}

void QEchoXProjectPanel::refreshOutliner()
{
	mOutliner->clear();
	for (IEchoXItem* item : mProject->getItems()) {
		QListWidgetItem* wItem = new QListWidgetItem(item->metaObject()->className());
		wItem->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue(item));
		mOutliner->addItem(wItem);
	}
}

void QEchoXProjectPanel::closeEvent(QCloseEvent* e)
{
	mProject->save();
}

void QEchoXProjectPanel::onItemDropped(QPoint center, IEchoXItem* item)
{
	mProject->addItem(item);
}

void QEchoXProjectPanel::onCurrentItemChanged(QListWidgetItem* widgetItem)
{
	if (widgetItem) {
		IEchoXItem* item = widgetItem->data(Qt::ItemDataRole::UserRole).value<IEchoXItem*>();
		if (item) {
			mDetailView->setObject(item);
			return;
		}
	}
	mDetailView->setObject(nullptr);
}
