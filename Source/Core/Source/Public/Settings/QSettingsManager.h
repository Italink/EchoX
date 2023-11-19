#ifndef QSettingsManager_h__
#define QSettingsManager_h__

#include <QObject>
#include <QHash>
#include "EchoXCoreAPI.h"

class IEchoXSettings;

class ECHOXCORE_API QSettingsManager : public QObject {
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
		mSettings.insert(&SettingsType::staticMetaObject, settings);
	}

	template<typename SettingsType>
	void unregisterSettings() {
		IEchoXSettings* settings = mSettings.take(&SettingsType::staticMetaObject);
		if (settings) {
			settings->deleteLater();
		}
	}
private:
	QSettingsManager();
private:
	QHash<const QMetaObject*, IEchoXSettings*> mSettings;
};

#endif // QSettingsManager_h__
