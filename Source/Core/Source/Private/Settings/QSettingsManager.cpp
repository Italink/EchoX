#include "QSettingsManager.h"
#include "IEchoXSettings.h"

QSettingsManager& QSettingsManager::Get()
{
	static QSettingsManager Instance;
	return Instance;
}

QList<IEchoXSettings*> QSettingsManager::getAllSettings()
{
	return mSettings.values();
}

QSettingsManager::QSettingsManager()
{

}
