#include "QSettingsManager.h"
#include "IEchoXSettings.h"

QSettingsManager& QSettingsManager::Get()
{
	static QSettingsManager Instance;
	return Instance;
}

void QSettingsManager::addSettings(const QMetaObject* inMetaObject, IEchoXSettings* inSettings)
{
	inSettings->setParent(this);
	mSettings << inSettings;
	mSettingsTypeMap.insert(inMetaObject, inSettings);
	inSettings->reload();
}

void QSettingsManager::removeSettings(const QMetaObject* inMetaObject)
{
	IEchoXSettings* settings = mSettingsTypeMap.take(inMetaObject);
	if (settings) {
		mSettings.removeOne(settings);
		settings->deleteLater();
	}
}

QList<IEchoXSettings*> QSettingsManager::getAllSettings()
{
	return mSettings;
}

QDir QSettingsManager::getSettingsDir() const
{
	if (!mSettingsDir.exists())
		mSettingsDir.mkpath(".");
	return mSettingsDir;
}

QSettingsManager::QSettingsManager()
{

}
