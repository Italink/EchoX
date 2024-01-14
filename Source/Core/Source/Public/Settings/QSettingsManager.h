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
		return qobject_cast<SettingsType*>(mSettingsTypeMap.value(&SettingsType::staticMetaObject));
	}

	template<typename SettingsType>
	void registerSettings() {
		SettingsType* settings = new SettingsType();
		addSettings(&SettingsType::staticMetaObject, settings);
		Q_EMIT asSettingsChanged();
	}

	template<typename SettingsType>
	void unregisterSettings() {
		removeSettings(&SettingsType::staticMetaObject);
		Q_EMIT asSettingsChanged();
	}
	void addSettings(const QMetaObject* inMetaObject, IEchoXSettings* inSettings);
	void removeSettings(const QMetaObject* inMetaObject);
	QList<IEchoXSettings*> getAllSettings();
	QDir getSettingsDir() const;
Q_SIGNALS:
	void asSettingsChanged();
private:
	QSettingsManager();
private:
	QDir mSettingsDir = QDir("./Settings");
	QList<IEchoXSettings*> mSettings;
	QHash<const QMetaObject*, IEchoXSettings*> mSettingsTypeMap;
};

#endif // QSettingsManager_h__
