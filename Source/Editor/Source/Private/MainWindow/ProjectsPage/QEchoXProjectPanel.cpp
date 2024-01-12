#include "QEchoXProjectPanel.h"
#include <QPushButton>
#include <QPainter>
#include <QMimeData>
#include <QApplication>
#include <QDrag>
#include <QScreen>
#include <QVBoxLayout>
#include "Utils/LoggingCategory.h"
#include "Project/Widget/IEchoXWidgetComponent.h"

QEchoXProjectPanel::QEchoXProjectPanel()
	: mBtReturn(new QPushButton("Return"))
	, mComponentSelector(new QEchoXComponentSelector)
	, mComponentOutliner(new QEchoXComponentOutliner)
{
	mBtReturn->setIcon(QIcon(":/Resources/return.png"));
	mBtReturn->setMaximumWidth(100);

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(5, 5, 5, 5);
	vLayout->addWidget(mBtReturn, 0, Qt::AlignLeft);

	QSplitter* hSplitter = new QSplitter;
	hSplitter->addWidget(mComponentSelector);
	hSplitter->setSizes({ 150, 600 });

	QSplitter* vSplitter = new QSplitter(Qt::Vertical);
	vSplitter->addWidget(mComponentOutliner);
	//vSplitter->addWidget(mDetailView);

	hSplitter->addWidget(vSplitter);

	vLayout->addWidget(hSplitter);
	vSplitter->setSizes({ 250,550 });

	connect(mBtReturn, &QPushButton::clicked, this, &QEchoXProjectPanel::asReturnBack);
	connect(mComponentSelector, &QEchoXComponentSelector::asItemDropped, this, &QEchoXProjectPanel::onComponentDropped);
	connect(mComponentOutliner,&QEchoXComponentOutliner::asComponentSelected, this, &QEchoXProjectPanel::onComponentSelected);
}

void QEchoXProjectPanel::closeEvent(QCloseEvent* e)
{
}

void QEchoXProjectPanel::onComponentDropped(QPoint center, IEchoXComponent* inComponent)
{
	QEchoXProject* project = QProjectsManager::Get().getCurrentProject();
	if (project) {
		project->addComponent(inComponent);
	}
}

void QEchoXProjectPanel::onComponentSelected(IEchoXComponent* inComponent)
{
}
