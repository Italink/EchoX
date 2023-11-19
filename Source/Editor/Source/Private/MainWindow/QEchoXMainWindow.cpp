#include "QEchoXMainWindow.h"
#include <QFileIconProvider>
#include <QVBoxLayout>
#include "framelessmanager.h"
#include "standardtitlebar.h"
#include "framelesswidgetshelper.h"
#include "framelesshelpercore_global.h"
#include "QEchoXNavigationBar.h"
#include "framelessconfig_p.h"

using namespace FRAMELESSHELPER_NAMESPACE;

QEchoXMainWindow::QEchoXMainWindow()
	: mTitleBar(new StandardTitleBar(this))
	, mNavigationBar (new QEchoXNavigationBar)
{
	initialize();
	mTimerId = startTimer(100);
	connect(FramelessManager::instance(), &FramelessManager::systemThemeChanged, this, &QEchoXMainWindow::updateStyleSheet);
}

QEchoXMainWindow::~QEchoXMainWindow()
{

}

void QEchoXMainWindow::initialize()
{
	setWindowTitle(tr("EchoX"));
	setWindowIcon(QFileIconProvider().icon(QFileIconProvider::Network));

	mTitleBar->setWindowIconVisible(true);
	mNavigationBar->addItem("Project", ":/Resources/projects.png", nullptr);
	mNavigationBar->addItem("Plugin", ":/Resources/plugin.png", nullptr);
	mNavigationBar->addItem("Setting", ":/Resources/setting-fill.png", nullptr);

	auto mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(mNavigationBar);

	const auto subLayout = new QVBoxLayout();
	subLayout->addWidget(mTitleBar);
	subLayout->addStretch();
	mainLayout->addLayout(subLayout);

	FramelessWidgetsHelper* helper = FramelessWidgetsHelper::get(this);
	helper->setTitleBarWidget(mTitleBar);
	//helper->setHitTestVisible(mNavigationBar, true);
	helper->setSystemButton(mTitleBar->minimizeButton(), Global::SystemButtonType::Minimize);
	helper->setSystemButton(mTitleBar->maximizeButton(), Global::SystemButtonType::Maximize);
	helper->setSystemButton(mTitleBar->closeButton(), Global::SystemButtonType::Close);
}

void QEchoXMainWindow::waitReady()
{
	FramelessWidgetsHelper* helper = FramelessWidgetsHelper::get(this);
	helper->waitForReady();
	helper->moveWindowToDesktopCenter();
}

void QEchoXMainWindow::updateStyleSheet()
{
	const bool dark = (FramelessManager::instance()->systemTheme() == Global::SystemTheme::Dark);
	const QColor labelTextColor = (dark ? kDefaultWhiteColor : kDefaultBlackColor);
	const QString labelStyleSheet = FRAMELESSHELPER_STRING_LITERAL("background-color: transparent; color: %1;").arg(labelTextColor.name());
	if (FramelessWidgetsHelper::get(this)->isBlurBehindWindowEnabled()) {
		setStyleSheet(FRAMELESSHELPER_STRING_LITERAL("background-color: transparent;"));
	}
	else {
		const QColor windowBackgroundColor = (dark ? Global::kDefaultSystemDarkColor : Global::kDefaultSystemLightColor);
		setStyleSheet(FRAMELESSHELPER_STRING_LITERAL("background-color: %1;").arg(windowBackgroundColor.name()));
	}
	update();
}

