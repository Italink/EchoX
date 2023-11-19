#ifndef QEchoXNavigationBar_h__
#define QEchoXNavigationBar_h__

#include <QWidget>
#include <QVBoxLayout>
#include "Widgets/QSvgIcon.h"
#include "Widgets/QHoverWidget.h"

class QEchoXNavigationItem : public QHoverWidget {
public:
	QEchoXNavigationItem(const QString& inName, QString inIconPath, QWidget* inPage);
	QString getName() const { return mName; }
private:
	void paintEvent(QPaintEvent* event) override;
private:
	QString mName;
	QSharedPointer<QSvgIcon> mIcon;
	QWidget* mPage;
};

class QEchoXNavigationBar : public QWidget {   
	Q_OBJECT
public: 
	QEchoXNavigationBar();
	void addItem(const QString& inName, QString inIconPath, QWidget* inPage);
	void removeItem(QString inName);
Q_SIGNALS:
	void asCurrentItemChanged(QEchoXNavigationItem*);
private:
	void paintEvent(QPaintEvent* e) override;
private:
	const int mShadowWidth = 5;
	QVBoxLayout* mLayout;
	QEchoXNavigationItem* mCurrentItem = nullptr;
	QSharedPointer<QSvgIcon> mLogo;
};

#endif // QEchoXNavigationBar_h__
