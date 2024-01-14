file(GLOB_RECURSE PROJECT_SOURCE FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h  ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Source FILES ${PROJECT_SOURCE})

add_executable(EchoX
    ${PROJECT_SOURCE}
)

target_link_libraries(EchoX PRIVATE
    EchoXEditor
)

add_custom_command(TARGET EchoX POST_BUILD     
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                    $<TARGET_FILE_DIR:QSmtcServer>
                    $<TARGET_FILE_DIR:TracyClient>
                    $<TARGET_FILE_DIR:QWindowKit::Core>  
                    $<TARGET_FILE_DIR:QWindowKit::Quick>  
                    $<TARGET_FILE_DIR:EchoX>)     

qengine_make_copy_target(EchoXCopyDLLTarget)
add_dependencies(EchoX EchoXCopyDLLTarget)