#ifndef QEchoXNavigationBar_h__
#define QEchoXNavigationBar_h__

#include <QWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include "Widgets/QSvgIcon.h"
#include "Widgets/QHoverWidget.h"
#include <QTimer>

class QEchoXNavigationItem : public QHoverWidget {
	Q_OBJECT
public:
	QEchoXNavigationItem(const QString& inName, QString inIconPath, QWidget* inPage);
	QString getName() const { return mName; }
	QWidget* getPage() const { return mPage; };
Q_SIGNALS:
	void asMousePressed(QEchoXNavigationItem* item);
private:
	void mousePressEvent(QMouseEvent* e) override;
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
	QEchoXNavigationItem* addItem(const QString& inName, QString inIconPath, QWidget* inPage);
	void removeItem(QString inName);
	void setCurrentItem(QEchoXNavigationItem* inItem);
Q_SIGNALS:
	void asCurrentItemChanged(QEchoXNavigationItem*);
private:
	void paintEvent(QPaintEvent* e) override;
private:
	QVBoxLayout* mLayout;
	QEchoXNavigationItem* mCurrentItem = nullptr;
	QSharedPointer<QSvgIcon> mLogo;
	QImage mInnerShadow;
	QTimer mAnimTimer;
	float mAnimOffset = 0.0f;
	float mAnimSpeed = 5.0f;
};

#endif // QEchoXNavigationBar_h__
