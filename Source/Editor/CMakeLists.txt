qengine_add_target(EchoXEditor
    ENABLE_MOC
    PUBLIC_DEPENDENCY   
        Qt::CorePrivate
        Qt::Gui
        Qt::GuiPrivate
        Qt::Widgets
        Qt::WidgetsPrivate
        Qt::Multimedia
        EchoXCore
        FramelessHelper::Core
        FramelessHelper::Widgets
    QRC_FILE EchoXEditor.qrc
    SOURCE_GROUP " "
)
