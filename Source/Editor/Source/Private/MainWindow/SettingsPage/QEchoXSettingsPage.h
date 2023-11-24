#ifndef QEchoXSettingsPage_h__
#define QEchoXSettingsPage_h__

#include <QWidget>
#include <QTreeWidget>
#include "DetailView/QDetailView.h"

class QEchoXSettingsPage : public QWidget {
	Q_OBJECT
public:
	QEchoXSettingsPage();
	void refreshSettings();
	void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
private:
	QTreeWidget* mSettingsOutliner;
	QDetailView* mSettingsView;
};

#endif // QEchoXSettingsPage_h__
