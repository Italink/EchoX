#include "QEchoXProjectPanel.h"
#include <QPushButton>

QEchoXProjectPanel::QEchoXProjectPanel()
	: mBtReturn(new QPushButton("Return"))
	, mDetailView(new QDetailView)
	, mAssetView(new QPushButton("AssetView"))
	, mOutline(new QPushButton("Outline"))
	, mSplitter(new QSplitter)
{
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->addWidget(mBtReturn);
	vLayout->addWidget(mSplitter);

	mSplitter->addWidget(mAssetView);
	mSplitter->addWidget(mOutline);
	mSplitter->addWidget(mDetailView);

	connect(mBtReturn, &QPushButton::clicked, this, &QEchoXProjectPanel::asReturnBack);
}

void QEchoXProjectPanel::setProject(IEchoXProject* inProject)
{
	mProject = inProject;
}

IEchoXProject* QEchoXProjectPanel::getProject() const
{
	return mProject;
}
