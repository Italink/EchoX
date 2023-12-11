#include "IEchoXSettings.h"
#include "QSettingsManager.h"
#include <QMetaClassInfo>
#include "Serialization.h"
#include <QJsonDocument>
#include <QJsonObject>

QString IEchoXSettings::getName()
{
	return metaObject()->classInfo(metaObject()->indexOfClassInfo("ClassName")).value();
}

QFile IEchoXSettings::getSettingsFile()
{
	return QSettingsManager::Get().getSettingsDir().filePath(getName() + ".json");
}

bool IEchoXSettings::save()  
{
	QFile file = getSettingsFile();
	if (file.open(QFile::WriteOnly)) {
		QByteArray data = QJsonDocument(Serialization::toCbor(this).toJsonObject()).toJson(QJsonDocument::Indented);
		file.write(data);
		return true;
	}
	return false;
}

bool IEchoXSettings::reload()
{
	QFile file = getSettingsFile();
	if (file.open(QFile::ReadOnly)) {
		QByteArray data = file.readAll();
		QCborMap map = QCborMap::fromJsonObject(QJsonDocument::fromJson(data).object());
		Serialization::fromCbor(this, map);
		return true;
	}
	return false;
}
