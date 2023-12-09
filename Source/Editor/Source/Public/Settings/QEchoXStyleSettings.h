#ifndef QEchoXStyleSettings_h__
#define QEchoXStyleSettings_h__

#include "Settings/IEchoXSettings.h"
#include "QEngineEditorStyleManager.h"

class QEchoXStyleSettings : public IEchoXSettings {
	Q_OBJECT
	Q_PROPERTY(QColor ThemeColor READ getThemeColor WRITE setThemeColor NOTIFY asStyleChanged)
	Q_PROPERTY(float ShadowWidth READ getShadowWidth WRITE setShadowWidth NOTIFY asStyleChanged)
	Q_CLASSINFO("ShadowWidth", "Min=0,Max=20")
public:
	ECHOX_DECLARE_SETTINGS(QEchoXStyleSettings, Style);
Q_SIGNALS:
	void asStyleChanged();
public:
	QEchoXStyleSettings();

	QColor getThemeColor();
	void setThemeColor(QColor var);

	float getShadowWidth();
	void setShadowWidth(float var);

	void refreshDetailViewStyle();
private:
	QColor mThemeColor = QColor(112, 115, 222);
	float mShadowWidth = 5;
	QEngineEditorStyleManager::Palette mDetailViewPalette;
};

#endif // QEchoXStyleSettings_h__
