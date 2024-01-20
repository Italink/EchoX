#ifndef QEchoXController_h__
#define QEchoXController_h__

#include <QObject>

class QEchoXProjectsModel;
class QEchoXSettingsModel;
class QEchoXPluginsModel;
class QQuickText;
class QQuickStackView;
class QEchoXProject;

class QEchoXController : public QObject {
	Q_OBJECT


public:
	static QEchoXController* Get();

	Q_INVOKABLE void initialize(QQuickText* inPageNameText, QQuickStackView* inStackView);
	Q_INVOKABLE void openSettingsPage();
	Q_INVOKABLE void openProjectPage(QEchoXProject* inProject);
	Q_INVOKABLE void goBack();
protected:
	QEchoXController();
private:
	QQuickText* mPageNameText = nullptr;
	QQuickStackView* mStackView = nullptr;
	QEchoXProjectsModel* mProjectsModel = nullptr;
	QEchoXSettingsModel* mSettingsModel = nullptr;
	QEchoXPluginsModel* mPluginsModel = nullptr;
};

#endif // QEchoXController_h__