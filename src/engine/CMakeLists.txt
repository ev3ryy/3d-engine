cmake_minimum_required(VERSION 3.14)

project(EngineRuntime)

file(GLOB_RECURSE ENGINE_RUNTIME_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_RUNTIME_SOURCES})

add_library(EngineRuntime STATIC ${ENGINE_RUNTIME_SOURCES})

target_include_directories(EngineRuntime PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_SOURCE_DIR}/scene
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_SOURCE_DIR}/include/glfw/include
    ${CMAKE_SOURCE_DIR}/include/glm
    ${CMAKE_SOURCE_DIR}/include/assimp/include
    ${CMAKE_SOURCE_DIR}/include/bullet3/src
    ${Vulkan_INCLUDE_DIRS}
    "${PROJECT_SOURCE_DIR}/src/modules/core"
    "${PROJECT_SOURCE_DIR}/src/modules/logger"
    "${PROJECT_SOURCE_DIR}/src/modules/input"
    "${PROJECT_SOURCE_DIR}/src/modules/renderer"
)

target_link_libraries(EngineRuntime PUBLIC
    core
    logger
    input
    renderer
    glfw
    Vulkan::Vulkan
    assimp
    BulletCollision
    BulletDynamics
    LinearMath
    Bullet3Common
)

target_compile_options(EngineRuntime PRIVATE /utf-8)

target_compile_definitions(EngineRuntime PUBLIC ENGINE_BUILD)