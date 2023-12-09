#ifndef QEchoXSettingsPage_h__
#define QEchoXSettingsPage_h__

#include <QWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QStackedWidget>
#include <QMap>
#include "DetailView/QDetailView.h"

class IEchoXSettings;

class QEchoXSettingsPage : public QWidget {
	Q_OBJECT
public:
	QEchoXSettingsPage();
	void refreshSettings();
	void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
private:
	QSplitter* mSplitter;
	QTreeWidget* mSettingsOutliner;
	QWidget* mSettingsViewBox;
};

#endif // QEchoXSettingsPage_h__
