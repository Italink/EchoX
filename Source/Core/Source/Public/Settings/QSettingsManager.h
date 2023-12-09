#ifndef QSettingsManager_h__
#define QSettingsManager_h__

#include <QObject>
#include <QHash>
#include "EchoXCoreAPI.h"
#include <QDir>

class IEchoXSettings;

class ECHOXCORE_API QSettingsManager : public QObject {
	Q_OBJECT
public:
	static QSettingsManager& Get();
	template<typename SettingsType>
	SettingsType* GetSettings() {
		return qobject_cast<SettingsType*>(mSettings.value(&SettingsType::staticMetaObject));
	}

	template<typename SettingsType>
	void registerSettings() {
		SettingsType* settings = new SettingsType();
		settings->setParent(this);
		addSettings(&SettingsType::staticMetaObject, settings);
		Q_EMIT asSettingsChanged();
	}

	template<typename SettingsType>
	void unregisterSettings() {
		IEchoXSettings* settings = mSettings.take(&SettingsType::staticMetaObject);
		if (settings) {
			settings->deleteLater();
		}
		Q_EMIT asSettingsChanged();
	}
	void addSettings(const QMetaObject* inMetaObject, IEchoXSettings* inSettings);
	QList<IEchoXSettings*> getAllSettings();
	QDir getSettingsDir() const;
Q_SIGNALS:
	void asSettingsChanged();
private:
	QSettingsManager();
private:
	QDir mSettingsDir = QDir("./Settings");
	QHash<const QMetaObject*, IEchoXSettings*> mSettings;
};

#endif // QSettingsManager_h__
