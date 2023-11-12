#include "QSettingsManager.h"

QSettingsManager& QSettingsManager::Get()
{
	static QSettingsManager Instance;
	return Instance;
}

QSettingsManager::QSettingsManager()
{

}
