cmake_minimum_required(VERSION 3.12)

project(QSmtcManager CXX)

set(CMAKE_AUTOMOC ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")

find_package(Qt6 COMPONENTS Core Widgets Gui Network  REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCE FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h  ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${PROJECT_SOURCE})

add_executable(QSmtcManager
	${PROJECT_SOURCE}
)
target_link_libraries(QSmtcManager PRIVATE Qt6::Widgets Qt6::Gui Qt6::Network)

add_subdirectory(Server)

add_custom_command(TARGET QSmtcManager POST_BUILD     
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                    $<TARGET_FILE_DIR:QSmtcServer>
                    $<TARGET_FILE_DIR:QSmtcManager>)     

#add_custom_command(TARGET QSmtcManager POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        $<TARGET_RUNTIME_DLLS:QSmtcServer>
#        $<TARGET_FILE_DIR:QSmtcManager>)
