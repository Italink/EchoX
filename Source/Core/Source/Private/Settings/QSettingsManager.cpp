#include "QSettingsManager.h"
#include "IEchoXSettings.h"

QSettingsManager& QSettingsManager::Get()
{
	static QSettingsManager Instance;
	return Instance;
}

QSettingsManager::QSettingsManager()
{

}
