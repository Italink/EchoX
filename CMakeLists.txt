cmake_minimum_required(VERSION 3.12)

project(EchoX CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUGEDITOR Debug Release)
find_package(Qt6 COMPONENTS Core Widgets Gui Multimedia ShaderTools REQUIRED)

add_subdirectory(3rdParty)
add_subdirectory(Source/Core)
add_subdirectory(Source/Editor)

file(GLOB PLUGIN_DIR_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} Plugins/*)
foreach(PLUGIN_DIR ${PLUGIN_DIR_LIST})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_DIR})
        add_subdirectory(${PLUGIN_DIR})
    endif()
endforeach()

add_subdirectory(Source/Launch)