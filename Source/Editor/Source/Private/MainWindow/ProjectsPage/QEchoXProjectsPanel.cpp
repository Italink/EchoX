#include "QEchoXProjectsPanel.h"
#include "Project/QProjectsManager.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QCheckBox>
#include "QEchoXStyleSettings.h"

QEchoXProjectsPanel::QEchoXProjectsPanel()
	: mHeader(new QEchoXProjectsHeader)
	, mProjectListWidget(new QListWidget)
{
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->setAlignment(Qt::AlignTop);
	vLayout->addWidget(mHeader);
	vLayout->addWidget(mProjectListWidget);

	mProjectListWidget->setViewMode(QListView::IconMode);
	mProjectListWidget->setFrameShape(QFrame::NoFrame);
	mProjectListWidget->setFlow(QListView::LeftToRight);
	mProjectListWidget->setTextElideMode(Qt::ElideRight);
	mProjectListWidget->setResizeMode(QListView::Adjust);
	mProjectListWidget->setSelectionMode(QListView::SingleSelection);
	mProjectListWidget->setDragDropMode(QAbstractItemView::NoDragDrop);
	QPalette palette = mProjectListWidget->palette();
	palette.setColor(QPalette::ColorRole::Base, Qt::transparent);
	mProjectListWidget->setPalette(palette);
	mProjectListWidget->setSpacing(10);

	refreshProjects();

	connect(&QProjectsManager::Get(), &QProjectsManager::asProjectsChanged, this, [this]() { refreshProjects(); });
	connect(&QProjectsManager::Get(), &QProjectsManager::asProjectCreated, this, [this]() { refreshProjects(); });
	connect(&QProjectsManager::Get(), &QProjectsManager::asProjectRemoved, this, [this]() { refreshProjects(); });
	//connect(mBtCreateNew, &QPushButton::clicked, this, &QEchoXProjectsPanel::onCreateNewProject);
	connect(mProjectListWidget, &QListWidget::itemDoubleClicked, this, &QEchoXProjectsPanel::onItemDoubleClicked);
}

void QEchoXProjectsPanel::refreshProjects()
{
	const QList<IEchoXProject*>& projects = QProjectsManager::Get().getProjectList();
	for (auto key : mProjectItemMap.keys()) {
		if (!projects.contains(key)) {
			removeProject(key);
		}
	}

	for (int i = 0; i < projects.size(); i++) {
		 IEchoXProject* project = projects[i];
		 if (!mProjectItemMap.contains(project)) {
			 addProject(i, project);
		 }
		 else {
			 QListWidgetItem* item = mProjectListWidget->takeItem(mProjectListWidget->row(mProjectItemMap[project]));
			 mProjectListWidget->insertItem(i, item);
		 }
	}
	refreshIconSize();
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
	QSize iconSize = QSize(mIconWdith * mIconScaleFactor, mIconWdith * mIconScaleFactor) ;
	mProjectListWidget->setIconSize(iconSize);
	for (int i = 0; i < mProjectListWidget->count(); i++) {
		QListWidgetItem* item = mProjectListWidget->item(i);
		IEchoXProject* project = item->data(Qt::StatusTipRole).value<IEchoXProject* >();
		QPixmap thumbnail = project->getThumbnail();
		item->setIcon(thumbnail.scaled(iconSize));
		item->setSizeHint(iconSize + QSize(0, mTextHeight));
	}
}

QRect QEchoXProjectsPanel::getProjectGemotry(IEchoXProject* inProject)
{
	const QListWidgetItem* item = mProjectItemMap.value(inProject);
	if (item) {
		QRect rect = mProjectListWidget->visualItemRect(item);
		QPoint pos = mProjectListWidget->mapToParent(rect.topLeft());
		rect.moveTopLeft(pos);
		rect.setHeight(rect.height() - mTextHeight);
		return rect;
	}
	return QRect();
}

QListWidgetItem* QEchoXProjectsPanel::getProjectItem(IEchoXProject* inProject)
{
	return mProjectItemMap.value(inProject);
}

void QEchoXProjectsPanel::updateProjectItem(IEchoXProject* inProject)
{
	if (QListWidgetItem* item = getProjectItem(inProject)) {
		QSize iconSize = QSize() * mIconScaleFactor;
		item->setText(inProject->getProjectName());
		item->setIcon(inProject->getThumbnail());
		item->setSizeHint(iconSize + QSize(0, mTextHeight));
		item->setTextAlignment(Qt::AlignCenter);
	}
}

void QEchoXProjectsPanel::addProject(int index, IEchoXProject* inProject)
{
	QListWidgetItem* item = new QListWidgetItem;
	item->setData(Qt::StatusTipRole, QVariant::fromValue(inProject));
	mProjectItemMap[inProject] = item;
	updateProjectItem(inProject);
	mProjectListWidget->insertItem(index, item);
}

void QEchoXProjectsPanel::removeProject(IEchoXProject* inProject)
{
	if (QListWidgetItem* item = getProjectItem(inProject)) {
		mProjectListWidget->takeItem(mProjectListWidget->row(item));
		delete item;
	}
}

void QEchoXProjectsPanel::onCreateNewProject()
{
	QProjectsManager::Get().createProject("NewProject");
}

void QEchoXProjectsPanel::onItemDoubleClicked(QListWidgetItem* inItem)
{
	IEchoXProject* project = mProjectItemMap.key(inItem,nullptr);
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

QEchoXProjectsHeader::QEchoXProjectsHeader()
	: mBtCreateProject(new QPushButton("Create Project"))
	, mSearchEdit(new QLineEdit)
{
	setFixedHeight(100);
	mBtCreateProject->setFixedWidth(100);

	QPalette pal = palette();
	pal.setColor(QPalette::ColorRole::WindowText, QColor(100, 100, 100));
	setPalette(pal);

	QGridLayout* layout = new QGridLayout(this);

	layout->addWidget(mSearchEdit, 0, 0, 1, 6);

	layout->addWidget(new QLabel("Type:"), 1, 0);
	layout->addWidget(new QCheckBox("All"), 1, 1);
	layout->addWidget(new QCheckBox("Widget"), 1, 2);
	layout->addWidget(new QCheckBox("Wallpaper"), 1, 3);
	layout->addWidget(new QCheckBox("Scene"), 1, 4);

	layout->addWidget(new QLabel("Order:"), 2, 0);
	layout->addWidget(new QRadioButton("Name"), 2, 1);
	layout->addWidget(new QRadioButton("Create Time"), 2, 2);
	layout->addWidget(new QRadioButton("Modify Time"), 2, 3);

	layout->addWidget(mBtCreateProject, 1, 5, 2, 1);
}

void QEchoXProjectsHeader::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	float ShadowWidth = QEchoXStyleSettings::Get()->getShadowWidth();
	QColor shadowColor = QColor(150, 150, 150);
	QRect shadowRect(0, height() - ShadowWidth, width(), ShadowWidth);
	QLinearGradient shadowLinearGradient;
	shadowLinearGradient.setStart(shadowRect.topLeft());
	shadowLinearGradient.setFinalStop(shadowRect.bottomLeft());
	shadowLinearGradient.setColorAt(0, QColor(shadowColor.red(), shadowColor.green(), shadowColor.blue()));
	shadowLinearGradient.setColorAt(1, QColor(shadowColor.red(), shadowColor.green(), shadowColor.blue(), 0));
	painter.fillRect(shadowRect, shadowLinearGradient);
}
