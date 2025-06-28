# game_app/CMakeLists.txt

cmake_minimum_required(VERSION 3.14)

project(GameApp)

file(GLOB_RECURSE GAME_APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAME_APP_SOURCES})

add_executable(GameApp ${GAME_APP_SOURCES})

target_link_libraries(GameApp PRIVATE
    EngineRuntime
    core
    logger
    input
    glfw
    Vulkan::Vulkan
    assimp

    debug Gamed
    optimized Game
)

target_compile_options(GameApp PRIVATE /utf-8)