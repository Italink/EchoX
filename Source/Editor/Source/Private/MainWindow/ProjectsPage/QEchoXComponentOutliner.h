#ifndef QEchoXComponentOutliner_h__
#define QEchoXComponentOutliner_h__

#include "QTreeWidget"
#include "Project/IEchoXComponent.h"
#include "Widgets/QSvgIcon.h"

class QEchoXComponentOutliner : public QTreeWidget {
	Q_OBJECT
public:
	QEchoXComponentOutliner();
	void refreshOutliner();
Q_SIGNALS:
	void asComponentSelected(IEchoXComponent*);
protected:
	void drawRow(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const override;
	void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void onItemClicked(QTreeWidgetItem* item, int column);
private:
	QSharedPointer<QSvgIcon> mEyeClose;
	QSharedPointer<QSvgIcon> mEye;
};

#endif // QEchoXComponentOutliner_h__