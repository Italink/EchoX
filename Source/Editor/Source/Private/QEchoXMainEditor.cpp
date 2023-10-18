#include "QEchoXMainEditor.h"
#include <QFileIconProvider>
#include <QVBoxLayout>
#include "framelessmanager.h"
#include "standardtitlebar.h"
#include "framelesswidgetshelper.h"
#include "framelesshelpercore_global.h"

using namespace FRAMELESSHELPER_NAMESPACE;

QEchoXMainEditor::QEchoXMainEditor()
	:mTitleBar(new StandardTitleBar(this))
{
	initialize();
	mTimerId = startTimer(100);
	connect(FramelessManager::instance(), &FramelessManager::systemThemeChanged, this, &QEchoXMainEditor::updateStyleSheet);
}

QEchoXMainEditor::~QEchoXMainEditor()
{

}

void QEchoXMainEditor::initialize()
{
	setWindowTitle(tr("EchoX"));
	setWindowIcon(QFileIconProvider().icon(QFileIconProvider::Desktop));

	mTitleBar->setWindowIconVisible(true);

	const auto mainLayout = new QVBoxLayout(this);
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(mTitleBar);
	mainLayout->addStretch();

	FramelessWidgetsHelper* helper = FramelessWidgetsHelper::get(this);
	helper->setTitleBarWidget(mTitleBar);
	helper->setSystemButton(mTitleBar->minimizeButton(), Global::SystemButtonType::Minimize);
	helper->setSystemButton(mTitleBar->maximizeButton(), Global::SystemButtonType::Maximize);
	helper->setSystemButton(mTitleBar->closeButton(), Global::SystemButtonType::Close);
}

void QEchoXMainEditor::waitReady()
{
	FramelessWidgetsHelper* helper = FramelessWidgetsHelper::get(this);
	helper->waitForReady();
	helper->moveWindowToDesktopCenter();
}

void QEchoXMainEditor::updateStyleSheet()
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

