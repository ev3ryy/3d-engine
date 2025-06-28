cmake_minimum_required(VERSION 3.14)

project(Editor)

file(GLOB_RECURSE EDITOR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EDITOR_SOURCES})

add_executable(Editor ${EDITOR_SOURCES})

target_include_directories(Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Editor PRIVATE
    EngineRuntime
    EngineImgui
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

target_compile_options(Editor PRIVATE /utf-8)