#include "QSmtcSettings.h"
#include "QSmtcView.h"

QWidget* QSmtcSettings::createWidget()
{
	return new QSmtcView();
}
