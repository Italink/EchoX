#include "QEchoXController.h"
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include "QEchoXSettingsModel.h"
#include "PluginsPage/QEchoXPluginsModel.h"
#include "private/qquickstackview_p.h"
#include "private/qquicktext_p.h"

QEchoXController::QEchoXController()
	: mSettingsModel(new QEchoXSettingsModel)
	, mPluginsModel(new QEchoXPluginsModel)
{
}

QEchoXController* QEchoXController::Get()
{
	static QEchoXController Instance;
	return &Instance;
}

void QEchoXController::initialize(QQuickText* inPageNameText, QQuickStackView* inStackView)
{
	mPageNameText = inPageNameText;
	mStackView = inStackView;
	QQmlEngine* engine = qmlEngine(mStackView);
	engine->rootContext()->setContextProperty("SettingsModel", mSettingsModel);
	engine->rootContext()->setContextProperty("PluginsModel", mPluginsModel);
	connect(mStackView, &QQuickStackView::currentItemChanged, [this]() {
		if(QQuickItem* item = mStackView->currentItem())
			mPageNameText->setText(item->objectName());
	});
}

void QEchoXController::openSettingsPage()
{
	mStackView->pushItem(QUrl("qrc:///Resources/Qml/SettingsPage.qml"));
}

Q_INVOKABLE void QEchoXController::goBack()
{
	if (mStackView->depth() > 1) {
		mStackView->popCurrentItem();
	}
}
