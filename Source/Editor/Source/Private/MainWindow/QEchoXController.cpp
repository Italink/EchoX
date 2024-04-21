#include "QEchoXController.h"
#include <QDebug>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include "QEchoXProjectsModel.h"
#include "QEchoXComponentModel.h"
#include "QEchoXProjectModel.h"
#include "QEchoXSettingsModel.h"
#include "QEchoXPluginsModel.h"
#include "private/qquickstackview_p.h"
#include "private/qquicktext_p.h"
#include "Project/QEchoXProject.h"

QEchoXController::QEchoXController()
	: mProjectsModel(new QEchoXProjectsModel)
	, mComponentModel(new QEchoXComponentModel)
	, mCurrentProjectModel(new QEchoXProjectModel)
	, mSettingsModel(new QEchoXSettingsModel)
	, mPluginsModel(new QEchoXPluginsModel)
{
}

QEchoXController* QEchoXController::Get()
{
	static QEchoXController Instance;
	return &Instance;
}

void QEchoXController::initialize(QQuickWindow* window, QQuickText* inPageNameText, QQuickStackView* inStackView)
{
	mMainWindow = window;
	mPageNameText = inPageNameText;
	mStackView = inStackView;
	QQmlEngine* engine = qmlEngine(mStackView);
	engine->rootContext()->setContextProperty("ProjectsModel", mProjectsModel);
	engine->rootContext()->setContextProperty("ComponentModel", mComponentModel);
	engine->rootContext()->setContextProperty("CurrentProjectModel", mCurrentProjectModel);
	engine->rootContext()->setContextProperty("SettingsModel", mSettingsModel);
	engine->rootContext()->setContextProperty("PluginsModel", mPluginsModel);
	connect(mStackView, &QQuickStackView::currentItemChanged, [this]() {
		if(QQuickItem* item = mStackView->currentItem())
			mPageNameText->setText(item->objectName());
	});
	connect(mComponentModel, &QEchoXComponentModel::asItemDropped, this, [](QPoint center, IEchoXItem* inComponent) {
		QEchoXProject* project = QEchoXProjectsManager::Get().getCurrentProject();
		if (project) {
			project->addComponent(inComponent);
		}
	});
}

void QEchoXController::openSettingsPage()
{
	mStackView->pushItem(QUrl("qrc:///Resources/Qml/SettingsPage.qml"), {},QQuickStackView::PushTransition);
}

void QEchoXController::openProjectPage(QEchoXProject* inProject)
{
	QEchoXProjectsManager::Get().setCurrentProject(inProject);
	QQuickItem* item = mStackView->pushItem(QUrl("qrc:///Resources/Qml/ProjectPage.qml"), {}, QQuickStackView::PushTransition);
}

void QEchoXController::goBack()
{
	if (mStackView->depth() > 1) {
		mStackView->popCurrentItem();
	}
}

void QEchoXController::createNewProject()
{
	QEchoXProjectsManager::Get().createProject("NewProject");
}

void QEchoXController::requestActivateMainWindow()
{
	if (mMainWindow) {
		mMainWindow->requestActivate();
	}
}
