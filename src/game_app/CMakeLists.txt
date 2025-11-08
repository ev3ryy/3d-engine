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

set_target_properties(GameApp PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

add_custom_command(
    TARGET GameApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ARGS "${CMAKE_SOURCE_DIR}/shaders/cache" 
         "$<TARGET_FILE_DIR:GameApp>/shaders/cache" 
    COMMENT "Copying compiled shader cache to execution directory."
)


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