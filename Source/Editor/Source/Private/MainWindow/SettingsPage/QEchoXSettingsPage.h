#ifndef QEchoXSettingsPage_h__
#define QEchoXSettingsPage_h__

#include <QWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QStackedWidget>
#include <QMap>
#include "DetailView/QDetailView.h"
#include "QEchoXSettingsOutliner.h"

class IEchoXSettings;

class QEchoXSettingsPage : public QWidget {
	Q_OBJECT
public:
	QEchoXSettingsPage();
	void onSettingsSelected(IEchoXSettings* settings);
private:
	QSplitter* mSplitter;
	QEchoXSettingsOutliner* mSettingsOutliner;
	QWidget* mSettingsViewBox;
};

#endif // QEchoXSettingsPage_h__
