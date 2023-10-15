qengine_add_target(EchoXEditor
    ENABLE_MOC
    PUBLIC_DEPENDENCY   
        Qt::Gui
        Qt::GuiPrivate
        Qt::Widgets
        Qt::WidgetsPrivate
        Qt::Multimedia
        QEngineCore
        QEngineEditor
    SOURCE_GROUP " "
)
