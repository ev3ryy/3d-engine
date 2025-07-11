cmake_minimum_required(VERSION 3.14)

project(physics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PHYSICS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PHYSICS_SOURCES})

add_library(physics STATIC ${PHYSICS_SOURCES})

target_compile_options(physics PRIVATE /utf-8)
set_property(TARGET physics PROPERTY VS_STARTUP_PROJECT TRUE)

target_include_directories(physics PUBLIC
    ${CMAKE_SOURCE_DIR}/include/glm
    ${CMAKE_SOURCE_DIR}/include/spdlog/include
    ${CMAKE_SOURCE_DIR}/include/entt/src
)

target_compile_features(physics PUBLIC cxx_std_20)