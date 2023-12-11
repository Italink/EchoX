#include "QTabCard.h"
#include <QVBoxLayout>

QTabCard::QTabCard(const QString& name, QWidget* body)
	: mLbName(new QLabel(name))
	, mBody(body)
{
	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(5, 5, 5, 5);
	vLayout->addWidget(mLbName, 0, Qt::AlignTop | Qt::AlignLeft);
	vLayout->addWidget(mBody, 1);
}

