cmake_minimum_required(VERSION 3.10)
project(MyEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_MAKE_PROGRAM "C:/Program Files/ninja/ninja.exe")
# set(CMAKE_GENERATOR "Ninja")

#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")

add_compile_options(/utf-8)

add_subdirectory(${CMAKE_SOURCE_DIR}/include/glfw)

find_package(Vulkan REQUIRED)

add_subdirectory(src/modules/core)
add_subdirectory(src/modules/renderer)
add_subdirectory(src/modules/input)
add_subdirectory(src/modules/logger)

file(GLOB_RECURSE ENGINE_SOURCES
    ${PROJECT_SOURCE_DIR}/src/engine/*.cpp
    ${PROJECT_SOURCE_DIR}/src/engine/*.h
    ${PROJECT_SOURCE_DIR}/src/engine/*.hpp
)
source_group(TREE ${PROJECT_SOURCE_DIR}/src/engine FILES ${ENGINE_SOURCES})

file(GLOB IMGUI_SOURCES
    "${CMAKE_SOURCE_DIR}/include/imgui/*.cpp"
    "${CMAKE_SOURCE_DIR}/include/imgui/vulkan/*.cpp"
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC
    "${CMAKE_SOURCE_DIR}/include/imgui"
    "${CMAKE_SOURCE_DIR}/include/imgui/vulkan"
    "${CMAKE_SOURCE_DIR}/include/glfw/include"
    "${CMAKE_SOURCE_DIR}/include/glm"
    ${Vulkan_INCLUDE_DIRS}
)

add_executable(engine ${ENGINE_SOURCES})

target_link_libraries(engine PRIVATE
    core
    renderer
    input
    logger
    imgui
    glfw
    Vulkan::Vulkan
)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT engine)