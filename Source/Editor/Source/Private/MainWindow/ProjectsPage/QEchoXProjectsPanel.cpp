#include "QEchoXProjectsPanel.h"
#include "Project/QProjectsManager.h"
#include <QVBoxLayout>

QEchoXProjectsPanel::QEchoXProjectsPanel()
	: mBtCreateNew(new QPushButton("Create New"))
	, mListWidget(new QListWidget)
{
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->addWidget(mBtCreateNew);
	vLayout->addWidget(mListWidget);

	mListWidget->setViewMode(QListView::IconMode);
	mListWidget->setFrameShape(QFrame::NoFrame);
	mListWidget->setFlow(QListView::LeftToRight);
	mListWidget->setTextElideMode(Qt::ElideRight);
	mListWidget->setResizeMode(QListView::Adjust);
	mListWidget->setSelectionMode(QListView::SingleSelection);
	mListWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

	mListWidget->setSpacing(10);
	refreshProjects();

	connect(&QProjectsManager::Get(), &QProjectsManager::asProjectCreated, this, [this]() {refreshProjects(); });
	connect(&QProjectsManager::Get(), &QProjectsManager::asProjectRemoved, this, [this]() {refreshProjects(); });
	connect(mBtCreateNew, &QPushButton::clicked, this, &QEchoXProjectsPanel::onCreateNewProject);
	connect(mListWidget, &QListWidget::itemDoubleClicked, this, &QEchoXProjectsPanel::onItemDoubleClicked);
}

void QEchoXProjectsPanel::refreshProjects()
{
	const QList<QEchoXProject*>& projects = QProjectsManager::Get().getProjectList();
	for (auto key : mProjectItemMap.keys()) {
		if (!projects.contains(key)) {
			removeProject(key);
		}
	}

	for (int i = 0; i < projects.size(); i++) {
		 QEchoXProject* project = projects[i];
		 if (!mProjectItemMap.contains(project)) {
			 addProject(i, project);
		 }
		 else {
			 QListWidgetItem* item = mListWidget->takeItem(mListWidget->row(mProjectItemMap[project]));
			 mListWidget->insertItem(i, item);
		 }
	}
}

void QEchoXProjectsPanel::setIconScaleFactor(float inVar)
{
	if (inVar > 0 && inVar < 4) {
		mIconScaleFactor = inVar;
		refreshIconSize();
	}
}

void QEchoXProjectsPanel::refreshIconSize()
{
	QSize iconSize = QSize(mIconWdith * mIconScaleFactor, mIconWdith * mIconScaleFactor/ mIconAspectRatio) ;
	mListWidget->setIconSize(iconSize);
	for (int i = 0; i < mListWidget->count(); i++) {
		QListWidgetItem* item = mListWidget->item(i);
		QEchoXProject* project = item->data(Qt::StatusTipRole).value<QEchoXProject* >();
		QPixmap thumbnail = project->getThumbnail();
		item->setIcon(thumbnail.scaled(iconSize));
		item->setSizeHint(iconSize + QSize(0, mTextHeight));
	}
}

QRect QEchoXProjectsPanel::getProjectGemotry(QEchoXProject* inProject)
{
	const QListWidgetItem* item = mProjectItemMap.value(inProject);
	if (item) {
		QRect rect = mListWidget->visualItemRect(item);
		QPoint pos = mListWidget->mapToParent(rect.topLeft());
		rect.moveTopLeft(pos);
		rect.setHeight(rect.height() - mTextHeight);
		return rect;
	}
	return QRect();
}

QListWidgetItem* QEchoXProjectsPanel::getProjectItem(QEchoXProject* inProject)
{
	return mProjectItemMap.value(inProject);
}

void QEchoXProjectsPanel::updateProjectItem(QEchoXProject* inProject)
{
	if (QListWidgetItem* item = getProjectItem(inProject)) {
		QSize iconSize = QSize() * mIconScaleFactor;
		item->setText(inProject->getProjectName());
		item->setIcon(inProject->getThumbnail());
		item->setSizeHint(iconSize + QSize(0, mTextHeight));
		item->setTextAlignment(Qt::AlignCenter);
	}
}

void QEchoXProjectsPanel::addProject(int index, QEchoXProject* inProject)
{
	QListWidgetItem* item = new QListWidgetItem;
	item->setData(Qt::StatusTipRole, QVariant::fromValue(inProject));
	mProjectItemMap[inProject] = item;
	updateProjectItem(inProject);
	mListWidget->insertItem(index, item);
}

void QEchoXProjectsPanel::removeProject(QEchoXProject* inProject)
{
	if (QListWidgetItem* item = getProjectItem(inProject)) {
		mListWidget->takeItem(mListWidget->row(item));
		delete item;
	}
}

void QEchoXProjectsPanel::onCreateNewProject()
{
	QProjectsManager::Get().createProject("NewProject", QEchoXProject::Type::Unknown);
}

void QEchoXProjectsPanel::onItemDoubleClicked(QListWidgetItem* inItem)
{
	QEchoXProject* project = mProjectItemMap.key(inItem,nullptr);
	Q_EMIT asProjectDoubleClicked(project);
}

void QEchoXProjectsPanel::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier)) {
		if (event->angleDelta().y() > 0) {
			setIconScaleFactor(mIconScaleFactor + 0.1);
		}
		else {
			setIconScaleFactor(mIconScaleFactor - 0.1);
		}
	}
}

void QEchoXProjectsPanel::resizeEvent(QResizeEvent* event)
{
	mIconAspectRatio = event->size().width() /(float) event->size().height();
	refreshIconSize();
}
