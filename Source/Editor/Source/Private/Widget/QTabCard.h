#ifndef QTabCard_h__
#define QTabCard_h__

#include <QWidget>
#include <QLabel>

class QTabCard: public QWidget {
public:
	QTabCard(const QString& name,QWidget* body);
protected:
	QLabel* mLbName;
	QWidget* mBody;
};

#endif // QTabCard_h__
