cmake_minimum_required(VERSION 3.14)

project(Game)

file(GLOB_RECURSE GAME_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GAME_SOURCES})

add_library(Game SHARED ${GAME_SOURCES})

target_include_directories(Game PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(Game PUBLIC
    EngineRuntime
    core
    input
    logger
)

target_compile_definitions(Game PRIVATE GAME_EXPORTS)

if(MSVC)
    target_compile_options(Game PRIVATE /utf-8)
else()
    target_compile_options(Game PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS Game
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/game.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Game
)