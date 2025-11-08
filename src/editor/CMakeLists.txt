cmake_minimum_required(VERSION 3.14)

project(Editor)

file(GLOB_RECURSE EDITOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EDITOR_SOURCES})

add_executable(Editor ${EDITOR_SOURCES})

set_target_properties(Editor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_command(
    TARGET Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ARGS "${CMAKE_SOURCE_DIR}/shaders/cache" 
         "$<TARGET_FILE_DIR:Editor>/shaders/cache" 
    COMMENT "Copying compiled shader cache to execution directory."
)

target_include_directories(Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Editor PRIVATE
    EngineRuntime
    EngineImGui
    Game
    core
    logger
    input
    renderer
    glfw
    Vulkan::Vulkan
    assimp
)

target_compile_definitions(Editor PRIVATE ENGINE_EDITOR)

if(MSVC)
    target_compile_options(Editor PRIVATE /utf-8)
else()
    target_compile_options(Editor PRIVATE -Wall -Wextra -Wpedantic)
endif()