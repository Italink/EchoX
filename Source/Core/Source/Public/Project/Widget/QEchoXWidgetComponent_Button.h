#ifndef QEchoXWidgetComponent_Button_h__
#define QEchoXWidgetComponent_Button_h__

#include "IEchoXWidgetComponent.h"
#include "qvectornd.h"
#include <QColor>
#include <QImage>
#include <QPushButton>

class ECHOXCORE_API QEchoXWidgetComponent_Button: public IEchoXWidgetComponent{
	Q_OBJECT
	Q_PROPERTY(QPushButton* Button READ button)
public:
public:
	Q_INVOKABLE QEchoXWidgetComponent_Button();
private:
	QWidget* widget() { return mButton; };
	QSize desiredSize() override { return QSize(60, 60); }
	QPushButton* button() { return mButton; }
private:
	QPushButton* mButton;
};

#endif // QEchoXWidgetComponent_Button_h__
