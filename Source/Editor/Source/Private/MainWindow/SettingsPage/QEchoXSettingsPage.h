#ifndef QEchoXSettingsPage_h__
#define QEchoXSettingsPage_h__

#include <QWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QStackedWidget>
#include <QMap>
#include "Settings/QSettingsManager.h"
#include "Settings/IEchoXSettings.h"

class QEchoXSettingsModel;

class QEchoXSettingsPage : public QWidget {
	Q_OBJECT
public:
	QEchoXSettingsPage();
private:
    void onSettingsSelected(IEchoXSettings* settings);
private:
	QSplitter* mSplitter;
	QWidget* mSettingsViewBox;
	QEchoXSettingsModel* mSettingsModel = nullptr;
};

#endif // QEchoXSettingsPage_h__
