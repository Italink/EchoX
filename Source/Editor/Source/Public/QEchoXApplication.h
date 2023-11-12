#ifndef QEchoXApplication_h__
#define QEchoXApplication_h__

#include <QApplication>
#include <QSystemTrayIcon>
#include "EchoXEditorAPI.h"
#include "qvectornd.h"

class QRhi;
class QEchoXMainWindow;
class QEchoXMenu;

#undef qApp
#define qApp (static_cast<QEchoXApplication *>(QCoreApplication::instance()))

#define Q_PROPERTY_VAR(Type,Name)\
    Q_PROPERTY(Type Name READ get_##Name WRITE set_##Name) \
    Type get_##Name(){ return Name; } \
    void set_##Name(Type var){ \
        Name = var;  \
		qDebug()<<"Set "<<#Name<<": "<<var; \
    } \
    Type Name


class TestInlineGadget {
	Q_GADGET
public:
	TestInlineGadget() { qDebug() << "Create"; }
	~TestInlineGadget() { qDebug() << "Release"; }
	Q_PROPERTY_VAR(double, LimitedDouble) = 1;
	Q_PROPERTY_VAR(QString, Desc) = "This is inline Gadget";
};

class TestInlineObject : public QObject {
	Q_OBJECT
public:
	Q_PROPERTY_VAR(QString, Desc) = "This is inline Object";
};

static QDebug operator<<(QDebug debug, const TestInlineGadget& gadget) {
	return debug << gadget.LimitedDouble << gadget.Desc;
}

static QDebug operator<<(QDebug debug, const std::shared_ptr<TestInlineGadget>& gadget) {
	return debug << gadget->LimitedDouble << gadget->Desc;
}

class TestObject :public QObject {
	Q_OBJECT
public:
	enum TestEnum {
		One,
		Two,
		Three
	};
	Q_ENUM(TestEnum);

	Q_PROPERTY_VAR(int, Int) = 0;
	Q_PROPERTY_VAR(float, Float) = 1.23f;
	Q_PROPERTY_VAR(double, LimitedDouble) = 5;
	Q_PROPERTY_VAR(TestEnum, Enum) = TestEnum::Two;
	Q_PROPERTY_VAR(QString, QtString);
	Q_PROPERTY_VAR(QString, AsMultiLineString);
	Q_PROPERTY_VAR(QString, AsPath);
	Q_PROPERTY_VAR(QString, AsCombo) = "A";
	Q_PROPERTY_VAR(QVector2D, Vec2);
	Q_PROPERTY_VAR(QVector3D, Vec3);
	Q_PROPERTY_VAR(QVector4D, Vec4);
	Q_PROPERTY_VAR(QColor, Color);
	Q_PROPERTY_VAR(QList<QColor>, ColorList) = { Qt::red,Qt::green,Qt::blue };
	Q_PROPERTY_VAR(std::vector<QColor>, StdColorList) = { Qt::red,Qt::green,Qt::blue };

	Q_PROPERTY(QMap<QString, QColor> ColorMap READ GetColorMap WRITE SetColorMap)

	QMap<QString, QColor> GetColorMap() const { return ColorMap; }
	void SetColorMap(QMap<QString, QColor> val) {
		ColorMap = val;
		qDebug() << "ColorMap: " << ColorMap;
	}
	QMap<QString, QColor> ColorMap = { {"Red",Qt::red},{"Green",Qt::green},{"Blue",Qt::blue} };

	Q_PROPERTY_VAR(TestInlineGadget, InlineGadget);
	Q_PROPERTY_VAR(TestInlineGadget*, InlineGadgetPtr) = new TestInlineGadget;
	//Q_PROPERTY_VAR(QSharedPointer<TestInlineGadget>, InlineGadgetSPtr) = QSharedPointer<TestInlineGadget>::create();
	Q_PROPERTY_VAR(TestInlineObject*, InlineObject) = new TestInlineObject;
	Q_PROPERTY_VAR(QSharedPointer<TestInlineObject>, InlineObjectSPtr) = QSharedPointer<TestInlineObject>::create();
	Q_PROPERTY_VAR(QList<TestInlineObject*>, InlineObjectList) = { };
};

Q_DECLARE_METATYPE(TestInlineGadget*);
Q_DECLARE_METATYPE(QSharedPointer<TestInlineGadget>);
Q_DECLARE_METATYPE(std::shared_ptr<TestInlineGadget>);

class ECHOXEDITOR_API QEchoXApplication : public QApplication {
public:
	QEchoXApplication(int& argc, char** argv);
	~QEchoXApplication();
	static void preInitialize();

	QRhi* getGlobalRhi() const;
	QEchoXMainWindow* getMainEditor() const;
	QSystemTrayIcon* getSystemTrayIcon() const;
protected:
	bool notify(QObject* o, QEvent* e) override;
	void onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
private:
	QSharedPointer<QRhi> mRhi;
	QEchoXMainWindow* mMainWindow = nullptr;
	QSystemTrayIcon* mSysIcon = nullptr;
	QEchoXMenu* mSysTrayMenu = nullptr;
};

#endif // QEchoXApplication_h__
