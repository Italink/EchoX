#include "QEchoXComponentModel.h"
#include "QEchoXController.h"
#include <QPainter>
#include <QWheelEvent>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include <QScreen>
#include "Project/Widget/IEchoXWidgetItem.h"
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
		this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint /*| Qt::Tool*/);
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

QEchoXComponentModel::QEchoXComponentModel(QObject* parent)
	: QAbstractItemModel(parent) {
    refresh();
    connect(&QEchoXProjectsManager::Get(), &QEchoXProjectsManager::asItemTypeInfoChanged, this, &QEchoXComponentModel::refresh);
}

QVariant QEchoXComponentModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();
    const QMetaObject* componentType = static_cast<QMetaObject*>(index.internalPointer());
    return componentType->className();
}

Qt::ItemFlags QEchoXComponentModel::flags(const QModelIndex& index) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex QEchoXComponentModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    if (!parent.isValid()) {
        return createIndex(row, column, QEchoXProjectsManager::Get().getItemTypeInfos()[row].metaObject);
    }
    return QModelIndex();
}

QModelIndex QEchoXComponentModel::parent(const QModelIndex& index) const {
    return QModelIndex();
}

int QEchoXComponentModel::rowCount(const QModelIndex& parent) const {
    if (!parent.isValid())
        return QEchoXProjectsManager::Get().getItemTypeInfos().size();
    return 0;
}

void QEchoXComponentModel::refresh() {
    beginResetModel();
    endResetModel();
}

void QEchoXComponentModel::notifyBeginDrag(int index){
    const QEchoXProjectsManager::ItemTypeInfo& comp = QEchoXProjectsManager::Get().getItemTypeInfos().value(index);
	if (IEchoXItem* item = QEchoXProjectsManager::Get().createItemByName(comp.name)) {
		QDrag drag(this);
		drag.setMimeData(this->mimeData({ this->index(index,0)}));
		QPixmap pixmap;
		IEchoXWidgetItem* widgetItem = qobject_cast<IEchoXWidgetItem*>(item);
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
		QEchoXDropWidget* droper = new QEchoXDropWidget();
		droper->show();
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
		droper->close();
		droper->deleteLater();
	}
	else {
		qCWarning(EchoX) << "Create item faild";
	}
}

int QEchoXComponentModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QHash<int, QByteArray> QEchoXComponentModel::roleNames() const {
    return {
        { Roles::name,"name" },
    };
}
