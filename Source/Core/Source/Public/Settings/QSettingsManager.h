#ifndef QSettingsManager_h__
#define QSettingsManager_h__

#include <QObject>
#include <QHash>
#include "IEchoXSettings.h"
#include "EchoXCoreAPI.h"

class ECHOXCORE_API QSettingsManager : public QObject {
public:
	static QSettingsManager& Get();
	template<typename SettingsType>
	SettingsType* GetSettings() {
		return mSettings.value(&IEchoXSettings::staticMetaObject);
	}

	template<typename SettingsType>
	void registerSettings() {
		SettingsType* settings = new SettingsType();
		settings->setParent(this);
		mSettings.insert(&IEchoXSettings::staticMetaObject, settings);
	}

	template<typename SettingsType>
	void unregisterSettings() {
		IEchoXSettings* settings = mSettings.take(&IEchoXSettings::staticMetaObject);
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
