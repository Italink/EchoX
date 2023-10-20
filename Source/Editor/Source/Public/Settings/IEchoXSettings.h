#ifndef IEchoXSettings_h__
#define IEchoXSettings_h__

#include <QObject>

class IEchoXSettings: public QObject {
	Q_OBJECT
public:
	virtual QString path() = 0;
	virtual QWidget* createWidget();
};

#endif // IEchoXSettings_h__
