#ifndef QEchoXComponentSelector_h__
#define QEchoXComponentSelector_h__

#include "QTreeWidget"
#include "Project/IEchoXComponent.h"

class QEchoXComponentSelector : public QTreeWidget {
	Q_OBJECT
public:
	QEchoXComponentSelector();
protected:
	void drawRow(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const override;
	void refreshComponentList();
	void startDrag(Qt::DropActions supportedActions) override;
Q_SIGNALS:
	void asItemDropped(QPoint, IEchoXComponent*);
};

#endif // QEchoXComponentSelector_h__