cmake_minimum_required(VERSION 3.12)

project(EchoX CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUGEDITOR Debug Release)
find_package(Qt6 COMPONENTS Core Widgets Gui Multimedia ShaderTools REQUIRED)

set(QENGINE_SOURCE_GROUP_PREFIX "3rdParty/QEngineUtilities" CACHE INTERNAL "")

add_subdirectory(3rdParty/QEngineUtilities)
add_subdirectory(3rdParty/QSmtcManager)
add_subdirectory(Source/App)
add_subdirectory(Source/Editor)

set_target_properties(QSmtcManager PROPERTIES FOLDER "3rdParty")
set_target_properties(QSmtcServer PROPERTIES FOLDER "3rdParty")