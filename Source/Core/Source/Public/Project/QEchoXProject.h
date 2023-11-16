#ifndef QEchoXProject_h__
#define QEchoXProject_h__

#include <QObject>
#include <QPixmap>
#include <QDir>
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QEchoXProject : public QObject {
	Q_OBJECT
public:
	inline static QString ConfgiFileName = "Project.echox";
	QEchoXProject(QString inName);

	QString getProjectName() const;
	bool rename(QString inNewName);

	QFile getProjectConfigFile() const;

	QDir getProjectDir() const;
	bool makeProjectDir();
	bool removeProjectDir();

	QPixmap getThumbnail();

	bool saveProject();

 private:
	QPixmap mThumbnail;
};

#endif // QEchoXProject_h__
