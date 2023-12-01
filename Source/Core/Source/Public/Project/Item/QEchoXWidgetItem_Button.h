#ifndef QEchoXWidgetItem_Button_h__
#define QEchoXWidgetItem_Button_h__

#include "IEchoXWidgetItem.h"
#include "qvectornd.h"
#include <QColor>
#include <QImage>
#include <QPushButton>

class ECHOXCORE_API QEchoXWidgetItem_Button: public IEchoXWidgetItem {
	Q_OBJECT
	Q_PROPERTY(QPushButton* Button READ button)
public:
public:
	Q_INVOKABLE QEchoXWidgetItem_Button();
private:
	QWidget* widget() { return mButton; };
	QSize desiredSize() override { return QSize(60, 60); }
	QPushButton* button() { return mButton; }
private:
	QPushButton* mButton;
};

#endif // QEchoXWidgetItem_Button_h__
