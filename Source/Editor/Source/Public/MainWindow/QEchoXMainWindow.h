#ifndef QEchoXMainWindow_h__
#define QEchoXMainWindow_h__

#include <QWidget>
#include <FramelessHelper/Widgets/framelesswidget.h>
#include "EchoXEditorAPI.h"
#include <QVBoxLayout>
#include <QStackedWidget>

class QEchoXNavigationBar;
class QEchoXNavigationItem;
FRAMELESSHELPER_BEGIN_NAMESPACE
class StandardTitleBar;
FRAMELESSHELPER_END_NAMESPACE

class ECHOXEDITOR_API QEchoXMainWindow : public FRAMELESSHELPER_PREPEND_NAMESPACE(FramelessWidget) {
public:
	QEchoXMainWindow();
	~QEchoXMainWindow();
protected:
	void initialize();
	void addNavigationItem(const QString & inName, const QString & inIconPath, QWidget * inPage);
	void waitReady();
private:
	void showEvent(QShowEvent * e) override;
	void hideEvent(QHideEvent * event) override;
	void closeEvent(QCloseEvent * e) override;
	void paintEvent(QPaintEvent * e) override;
private Q_SLOTS:
	void updateStyleSheet();
private:
	FRAMELESSHELPER_PREPEND_NAMESPACE(StandardTitleBar)* mTitleBar = nullptr;
	QEchoXNavigationBar* mNavigationBar = nullptr;
	int mTimerId = -1;
	QWidget* mPlaceholderPage = nullptr;
	QStackedWidget* mBody = nullptr;
};

#endif