#ifndef QEchoXPluginsPage_h__
#define QEchoXPluginsPage_h__

#include <QWidget>
#include <QListWidget>
#include "DetailView/QDetailView.h"

class QEchoXPluginsPage : public QWidget {
	Q_OBJECT
public:
	QEchoXPluginsPage();
	void refreshPlugins();
	void onCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
private:
	QListWidget* mPluginOutliner;
};

#endif // QEchoXPluginsPage_h__
