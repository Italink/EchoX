#ifndef QEchoXProject_h__
#define QEchoXProject_h__

#include <QObject>
#include <QPixmap>
#include <QDir>
#include "IEchoXComponent.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QEchoXProject : public QObject {
	Q_OBJECT
	Q_PROPERTY(QList<IEchoXComponent*> Items READ getItems WRITE setItems)
public:
	QEchoXProject();

	QString getProjectName() const;
	QFile getProjectFile() const;
	QList<IEchoXComponent*> getItems() const;
	QPixmap getThumbnail();

	void activate();
	void deactivate();
	bool rename(QString inNewName);
	bool save();
	void setItems(QList<IEchoXComponent*> val);
	void setThumbnail(QPixmap inPixmap);
	void addComponent(IEchoXComponent* inItem);
	void removeComponent(IEchoXComponent* inItem);
	const QList<IEchoXComponent*>& getComponents() const;
	void clearInvailedItem();
Q_SIGNALS:
	void asComponentsChanged();
private:
	bool bActivated = false;
	bool bModified = false;
	QPixmap mThumbnail;
	QList<IEchoXComponent*> mComponents;
};

#endif // QEchoXProject_h__
