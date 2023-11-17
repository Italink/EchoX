#ifndef IEchoXProject_h__
#define IEchoXProject_h__

#include <QObject>
#include <QPixmap>
#include <QDir>
#include "EchoXCoreAPI.h"

class ECHOXCORE_API IEchoXProject : public QObject {
	Q_OBJECT
public:
	IEchoXProject(QString inName);
	QString getProjectName() const;
	bool rename(QString inNewName);

	QFile getProjectFile() const;
	bool saveProject();

	QPixmap getThumbnail();
 private:
	QPixmap mThumbnail;
};

#endif // IEchoXProject_h__
