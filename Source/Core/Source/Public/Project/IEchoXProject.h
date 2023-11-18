#ifndef IEchoXProject_h__
#define IEchoXProject_h__

#include <QObject>
#include <QPixmap>
#include <QDir>
#include "EchoXCoreAPI.h"

class ECHOXCORE_API IEchoXProject : public QObject {
	Q_OBJECT
public:
	IEchoXProject();
	QString getProjectName() const;
	bool rename(QString inNewName);

	QFile getProjectFile() const;
	bool save();

	void setThumbnail(QPixmap inPixmap);
	QPixmap getThumbnail();

	virtual QWidget* createProjectPanel();
 private:
	QPixmap mThumbnail;
};

#endif // IEchoXProject_h__
