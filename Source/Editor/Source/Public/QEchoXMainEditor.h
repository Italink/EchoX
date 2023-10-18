#ifndef QEchoXMainEditor_h__
#define QEchoXMainEditor_h__

#include <QWidget>
#include <FramelessHelper/Widgets/framelesswidget.h>
#include "EchoXEditorAPI.h"

FRAMELESSHELPER_BEGIN_NAMESPACE
class StandardTitleBar;
FRAMELESSHELPER_END_NAMESPACE

class ECHOXEDITOR_API QEchoXMainEditor : public FRAMELESSHELPER_PREPEND_NAMESPACE(FramelessWidget) {
public:
	QEchoXMainEditor();
	~QEchoXMainEditor();
protected:
	void initialize();
	void waitReady();

private Q_SLOTS:
	void updateStyleSheet();
private:
	FRAMELESSHELPER_PREPEND_NAMESPACE(StandardTitleBar)* mTitleBar = nullptr;
	int mTimerId = -1;
};

#endif