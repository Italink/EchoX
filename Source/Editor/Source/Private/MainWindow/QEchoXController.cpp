#include "QEchoXController.h"
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include "QEchoXProjectsModel.h"
#include "QEchoXSettingsModel.h"
#include "QEchoXPluginsModel.h"
#include "private/qquickstackview_p.h"
#include "private/qquicktext_p.h"
#include "Project/QEchoXProject.h"

QEchoXController::QEchoXController()
	: mProjectsModel(new QEchoXProjectsModel)
	, mSettingsModel(new QEchoXSettingsModel)
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
	engine->rootContext()->setContextProperty("ProjectsModel", mProjectsModel);
	engine->rootContext()->setContextProperty("SettingsModel", mSettingsModel);
	engine->rootContext()->setContextProperty("PluginsModel", mPluginsModel);
	connect(mStackView, &QQuickStackView::currentItemChanged, [this]() {
		if(QQuickItem* item = mStackView->currentItem())
			mPageNameText->setText(item->objectName());
	});
}

void QEchoXController::openSettingsPage()
{
	mStackView->pushItem(QUrl("qrc:///Resources/Qml/SettingsPage.qml"), {},QQuickStackView::PushTransition);
}

void QEchoXController::openProjectPage(QEchoXProject* inProject)
{
	QQuickItem* item = mStackView->pushItem(QUrl("qrc:///Resources/Qml/ProjectPage.qml"), {}, QQuickStackView::PushTransition);
}

void QEchoXController::goBack()
{
	if (mStackView->depth() > 1) {
		mStackView->popCurrentItem();
	}
}
