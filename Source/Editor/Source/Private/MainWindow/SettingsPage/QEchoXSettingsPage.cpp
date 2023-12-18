#include "QEchoXSettingsPage.h"
#include "Settings/QSettingsManager.h"
#include "Settings/IEchoXSettings.h"
#include <QMetaClassInfo>

QEchoXSettingsPage::QEchoXSettingsPage()
	: mSplitter(new QSplitter)
	, mSettingsOutliner(new QEchoXSettingsOutliner())
	, mSettingsViewBox(new QWidget)
{

	QHBoxLayout* hLayout = new QHBoxLayout(this);
	hLayout->addWidget(mSplitter);
	mSplitter->addWidget(mSettingsOutliner);
	mSplitter->addWidget(mSettingsViewBox);
	mSplitter->setSizes({ 300,800 });
	QHBoxLayout* viewLayout = new QHBoxLayout(mSettingsViewBox);
	connect(mSettingsOutliner, &QEchoXSettingsOutliner::asSettingsSelected, this, &QEchoXSettingsPage::onSettingsSelected);
}

void QEchoXSettingsPage::onSettingsSelected(IEchoXSettings* settings)
{
	if (settings) {
		while (QLayoutItem* item = mSettingsViewBox->layout()->takeAt(0)) {
			if (QWidget* widget = item->widget())
				widget->deleteLater();
			delete item;
		}
		QWidget* view = settings->createWidget();
		connect(view, &QObject::destroyed, settings, &IEchoXSettings::save);
		mSettingsViewBox->layout()->addWidget(view);
		return;
	}
}
