#include "QEchoXSettingsPage.h"
#include "Settings/QSettingsManager.h"
#include "Settings/IEchoXSettings.h"
#include <QMetaClassInfo>
#include <QQuickWidget>
#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include "Settings/QEchoXStyleSettings.h"
#include "QEchoXSettingsModel.h"

QEchoXSettingsPage::QEchoXSettingsPage()
	: mSplitter(new QSplitter)
	, mSettingsViewBox(new QWidget)
    , mSettingsModel(new QEchoXSettingsModel)
{
	QHBoxLayout* hLayout = new QHBoxLayout(this);
	hLayout->setContentsMargins(10, 10, 10, 10);
	hLayout->addWidget(mSplitter);
	QQuickWidget* quick = new QQuickWidget;

	mSettingsModel->refresh();

	quick->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
	quick->setAttribute(Qt::WA_AlwaysStackOnTop);
	quick->setClearColor(Qt::transparent);
	quick->rootContext()->setContextProperty("settingsModel", mSettingsModel);
	quick->rootContext()->setContextProperty("echoxStyle", QEchoXStyleSettings::Get());
	quick->setSource(QUrl("qrc:/Qml/SettingsPageOutliner.qml"));
	

	mSplitter->addWidget(quick);
	mSplitter->addWidget(mSettingsViewBox);
	mSplitter->setSizes({ 300,800 });
	QHBoxLayout* viewLayout = new QHBoxLayout(mSettingsViewBox);

    connect(&QSettingsManager::Get(), &QSettingsManager::asSettingsChanged, mSettingsModel, &QEchoXSettingsModel::refresh);
	connect(mSettingsModel, &QEchoXSettingsModel::settingsSelected, this, &QEchoXSettingsPage::onSettingsSelected);
}

void QEchoXSettingsPage::onSettingsSelected(IEchoXSettings* settings)
{
	while (QLayoutItem* item = mSettingsViewBox->layout()->takeAt(0)) {
		if (QWidget* widget = item->widget()) {
			widget->close();
			widget->setParent(nullptr);
			widget->deleteLater();
		}
		delete item;
	}
	if (settings) {
		QWidget* view = settings->createWidget();
		view->setAttribute(Qt::WA_DeleteOnClose);
		connect(view, &QObject::destroyed, settings, &IEchoXSettings::save);
		mSettingsViewBox->layout()->addWidget(view);
	}
}
