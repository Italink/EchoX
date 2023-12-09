#include "QSettingsManager.h"
#include "IEchoXSettings.h"

QSettingsManager& QSettingsManager::Get()
{
	static QSettingsManager Instance;
	return Instance;
}

void QSettingsManager::addSettings(const QMetaObject* inMetaObject, IEchoXSettings* inSettings)
{
	mSettings.insert(inMetaObject, inSettings);
	inSettings->reload();
}

QList<IEchoXSettings*> QSettingsManager::getAllSettings()
{
	return mSettings.values();
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
