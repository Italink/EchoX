#ifndef QEchoXTrayMenu_h__
#define QEchoXTrayMenu_h__

#include <QMenu>
#include "EchoXEditorAPI.h"

class ECHOXEDITOR_API QEchoXMenu: public QMenu {
public:
	QEchoXMenu(QWidget* parent = nullptr);
private:
	void paintEvent(QPaintEvent* e) override;
};

#endif // QEchoXTrayMenu_h__