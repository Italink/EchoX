#include "QEchoXProjectPanel.h"
#include <QPushButton>

QEchoXProjectPanel::QEchoXProjectPanel()
	: mBtReturn(new QPushButton("Return"))
{
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->addWidget(mBtReturn);

	//mSplitter->addWidget(mAssetView);
	//mSplitter->addWidget(mOutline);
	//mSplitter->addWidget(mDetailView);

	connect(mBtReturn, &QPushButton::clicked, this, &QEchoXProjectPanel::asReturnBack);
}

void QEchoXProjectPanel::setProject(IEchoXProject* inProject)
{
	if (mCurrProjectWidget) {
		layout()->removeWidget(mCurrProjectWidget);
		mCurrProjectWidget->deleteLater();
	}
	mCurrProjectWidget = inProject->createProjectPanel();
	layout()->addWidget(mCurrProjectWidget);
	mProject = inProject;
}

IEchoXProject* QEchoXProjectPanel::getProject() const
{
	return mProject;
}

void QEchoXProjectPanel::closeEvent(QCloseEvent* e)
{
	mProject->save();
}
