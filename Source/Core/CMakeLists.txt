qengine_add_target(EchoXCore
    ENABLE_MOC
    PUBLIC_DEPENDENCY   
        Qt::Core
        Qt::CorePrivate
        Qt::Multimedia
        QEngineCore
        QEngineEditor
        Gist
    SOURCE_GROUP " "
)
