#ifndef QEchoXProject_h__
#define QEchoXProject_h__

#include <QObject>
#include <QPixmap>
#include <QDir>
#include "IEchoXItem.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QEchoXProject : public QObject {
	Q_OBJECT
	Q_PROPERTY(QList<IEchoXItem*> Items READ getItems WRITE setItems)
public:
	QEchoXProject();

	QString getProjectName() const;
	QFile getProjectFile() const;
	QList<IEchoXItem*> getItems() const;
	QPixmap getThumbnail();

	void activate();
	void deactivate();
	bool rename(QString inNewName);
	bool save();
	void setItems(QList<IEchoXItem*> val);
	void setThumbnail(QPixmap inPixmap);
	void addItem(IEchoXItem* inItem);
	void removeItem(IEchoXItem* inItem);
Q_SIGNALS:
	void asItemsChanged();
private:
	bool bActivated = false;
	QPixmap mThumbnail;
	QList<IEchoXItem*> mItems;
};

#endif // QEchoXProject_h__
