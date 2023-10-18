file(GLOB_RECURSE PROJECT_SOURCE FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h  ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${PROJECT_SOURCE})

add_executable(EchoX
    ${PROJECT_SOURCE}
)

target_link_libraries(EchoX PRIVATE
    EchoXEditor
)

qengine_make_copy_target(EchoXCopyDLLTarget)
add_dependencies(EchoX EchoXCopyDLLTarget)