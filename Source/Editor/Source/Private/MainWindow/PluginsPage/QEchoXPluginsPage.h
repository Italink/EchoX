#ifndef QEchoXPluginsPage_h__
#define QEchoXPluginsPage_h__

#include <QQuickWidget>
#include <QListWidget>
#include "DetailView/QDetailView.h"

class QEchoXPluginsModel;

class QEchoXPluginsPage : public QWidget {
	Q_OBJECT
public:
	QEchoXPluginsPage();
	QEchoXPluginsModel* mPluginsModel = nullptr;
};

#endif // QEchoXPluginsPage_h__
