#ifndef QEchoXStyleSettings_h__
#define QEchoXStyleSettings_h__

#include "Settings/IEchoXSettings.h"
#include "QEngineEditorStyleManager.h"

class QEchoXStyleSettings : public IEchoXSettings {
	Q_OBJECT
	Q_PROPERTY(QColor ThemeColor READ getThemeColor WRITE setThemeColor NOTIFY asStyleChanged)
	Q_PROPERTY(QColor HoveredColor READ getHoveredColor WRITE setHoveredColor NOTIFY asStyleChanged USER true)
	Q_PROPERTY(QColor SelectedColor READ getSelectedColor WRITE setSelectedColor NOTIFY asStyleChanged USER true)
	Q_PROPERTY(float ShadowWidth READ getShadowWidth WRITE setShadowWidth NOTIFY asStyleChanged )
	Q_CLASSINFO("ShadowWidth", "Min=0,Max=20")
public:
	ECHOX_DECLARE_SETTINGS(QEchoXStyleSettings, Style);
Q_SIGNALS:
	void asStyleChanged();
public:
	QEchoXStyleSettings();

	Q_INVOKABLE QColor getThemeColor();
	Q_INVOKABLE void setThemeColor(QColor var);

	Q_INVOKABLE QColor getHoveredColor();
	Q_INVOKABLE void setHoveredColor(QColor var);

	Q_INVOKABLE QColor getSelectedColor();
	Q_INVOKABLE void setSelectedColor(QColor var);


	Q_INVOKABLE float getShadowWidth();
	Q_INVOKABLE void setShadowWidth(float var);

	void refreshDetailViewStyle();

	QByteArray getStyleSheet() const;
private:
	QColor mThemeColor = QColor(112, 115, 222);
	float mShadowWidth = 5;
	QEngineEditorStyleManager::Palette mDetailViewPalette;
};

#endif // QEchoXStyleSettings_h__
