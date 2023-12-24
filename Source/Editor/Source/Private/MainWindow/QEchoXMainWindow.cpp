#include "QEchoXMainWindow.h"
#include <QFileIconProvider>
#include <QVBoxLayout>
#include "framelessmanager.h"
#include "standardtitlebar.h"
#include "framelesswidgetshelper.h"
#include "framelesshelpercore_global.h"
#include "QEchoXNavigationBar.h"
#include "framelessconfig_p.h"
#include "QEchoXProjectsPage.h"
#include "QEchoXStyleSettings.h"
#include "SettingsPage/QEchoXSettingsPage.h"
#include "PluginsPage/QEchoXPluginsPage.h"
#include "Project/Widget/3D/QWindow3DEditor.h"
#include "Project/Widget/VFX/QWidgetVFXManager.h"

using namespace FRAMELESSHELPER_NAMESPACE;

QEchoXMainWindow::QEchoXMainWindow()
	: mTitleBar(new StandardTitleBar(this))
	, mNavigationBar (new QEchoXNavigationBar)
	, mPlaceholderPage(new QWidget)
	, mBody(new QStackedWidget)
{
	initialize();
	mTimerId = startTimer(100);

	connect(FramelessManager::instance(), &FramelessManager::systemThemeChanged, this, &QEchoXMainWindow::updateStyleSheet);
	connect(mNavigationBar, &QEchoXNavigationBar::asCurrentItemChanged, this, [this](QEchoXNavigationItem* item) {
		mBody->setCurrentWidget(item ? item->getPage() : mPlaceholderPage);
	});
	connect(QWindow3DEditor::Instance(), &QWindow3DEditor::asClicked, this, &QWidget::activateWindow);
	connect(QEchoXStyleSettings::Get(), &QEchoXStyleSettings::asStyleChanged, this, &QEchoXMainWindow::updateStyleSheet);
}

QEchoXMainWindow::~QEchoXMainWindow()
{

}

void QEchoXMainWindow::initialize()
{
	setWindowTitle(tr("EchoX"));
	setWindowIcon(QFileIconProvider().icon(QFileIconProvider::Network));
	setWindowFlags(Qt::WindowStaysOnTopHint);

	mTitleBar->setWindowIconVisible(false);
	mTitleBar->setTitleLabelVisible(false);
	mTitleBar->chromePalette()->setTitleBarActiveBackgroundColor(Qt::transparent);
	mTitleBar->chromePalette()->setTitleBarInactiveBackgroundColor(Qt::transparent);

	addNavigationItem("Project", ":/Resources/projects.png", new QEchoXProjectsPage);
	addNavigationItem("Plugin", ":/Resources/plugin.png", new QEchoXPluginsPage);
	addNavigationItem("Setting", ":/Resources/setting-fill.png", new QEchoXSettingsPage);

	auto mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(mNavigationBar);

	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->setAlignment(Qt::AlignTop);
	vLayout->setSpacing(0);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->addWidget(mTitleBar);
	vLayout->addWidget(mBody);
	mainLayout->addLayout(vLayout);

	QPalette pal = palette();
	pal.setColor(QPalette::ColorRole::Window, Qt::white);
	setPalette(pal);

	FramelessWidgetsHelper* helper = FramelessWidgetsHelper::get(this);
	helper->setTitleBarWidget(mTitleBar);
	//helper->setHitTestVisible(mNavigationBar, true);
	helper->setSystemButton(mTitleBar->minimizeButton(), Global::SystemButtonType::Minimize);
	helper->setSystemButton(mTitleBar->maximizeButton(), Global::SystemButtonType::Maximize);
	helper->setSystemButton(mTitleBar->closeButton(), Global::SystemButtonType::Close);

	updateStyleSheet();
}

void QEchoXMainWindow::addNavigationItem(const QString& inName, const QString& inIconPath, QWidget* inPage)
{
	mNavigationBar->addItem(inName, inIconPath, inPage);
	mBody->addWidget(inPage);
	QPalette pal = inPage->palette();
	pal.setColor(QPalette::ColorRole::Base, Qt::transparent);
	inPage->setPalette(pal);
}

void QEchoXMainWindow::waitReady()
{
	FramelessWidgetsHelper* helper = FramelessWidgetsHelper::get(this);
	helper->waitForReady();
	helper->moveWindowToDesktopCenter();
}

void QEchoXMainWindow::showEvent(QShowEvent* e)
{
	//QWindow3DEditor::Instance()->show();
	activateWindow();
}

void QEchoXMainWindow::hideEvent(QHideEvent* event)
{
	QWindow3DEditor::Instance()->hide();
}

void QEchoXMainWindow::closeEvent(QCloseEvent* e)
{
	QWindow3DEditor::Instance()->hide();
	QWidgetVFXManager::Get().playWidgetCloseVFX(this);
}

void QEchoXMainWindow::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	float ShadowWidth = QEchoXStyleSettings::Get()->getShadowWidth();
	QColor shadowColor = QColor(150, 150, 150);
	QRect shadowRect(mNavigationBar->width(), 0, ShadowWidth, height());
	QLinearGradient shadowLinearGradient;
	shadowLinearGradient.setStart(shadowRect.topLeft());
	shadowLinearGradient.setFinalStop(shadowRect.topRight());
	shadowLinearGradient.setColorAt(0, QColor(shadowColor.red(), shadowColor.green(), shadowColor.blue()));
	shadowLinearGradient.setColorAt(1, QColor(shadowColor.red(), shadowColor.green(), shadowColor.blue(), 0));
	painter.fillRect(shadowRect, shadowLinearGradient);
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
	setStyleSheet(QEchoXStyleSettings::Get()->getStyleSheet());
	update();
}

