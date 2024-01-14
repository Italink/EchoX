#include "IEchoXSettings.h"
#include "QSettingsManager.h"
#include <QMetaClassInfo>
#include "Serialization.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "DetailView/QQuickDetailsView.h"

QQuickItem* IEchoXSettings::createView(QQmlEngine* engine)
{
	QQmlComponent component = QQmlComponent(engine);
	component.setData(R"(
		import QtQuick
		import QtQuick.Controls
		import QtQuick.Layouts
		import Qt.DetailsView
		DetailsView {
            id: detailsView
            Layout.fillWidth : true
            Layout.fillHeight: true
        }
	)",QUrl());
	QQuickDetailsView* detailsView = qobject_cast<QQuickDetailsView*>(component.create());
	detailsView->setObject(this);
	return detailsView;
}

QString IEchoXSettings::getName() const
{
	return metaObject()->classInfo(metaObject()->indexOfClassInfo("ClassName")).value();
}

QFile IEchoXSettings::getSettingsFile() const
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
