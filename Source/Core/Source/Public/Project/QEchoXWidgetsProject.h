#ifndef QEchoXWidgetsProject_h__
#define QEchoXWidgetsProject_h__

#include "IEchoXProject.h"

class QEchoXWidgetsProject : public IEchoXProject {
	Q_OBJECT
	Q_PROPERTY(int value READ getValue WRITE setValue)
public:
	Q_INVOKABLE QEchoXWidgetsProject() {};
	int getValue() const { return value; }
	void setValue(int val) { value = val; }
private:
	int value = 5;
};

#endif // QEchoXWidgetsProject_h__
