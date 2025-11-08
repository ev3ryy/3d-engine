cmake_minimum_required(VERSION 3.14)

if (DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()
project(MyEngine)

message(STATUS "VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_SOURCE_DIR}/include/glfw)
add_subdirectory(${CMAKE_SOURCE_DIR}/include/assimp)
add_subdirectory(${CMAKE_SOURCE_DIR}/include/bullet3 EXCLUDE_FROM_ALL)

find_package(Vulkan REQUIRED)

find_program(DXC_EXECUTABLE dxc)

if(NOT DXC_EXECUTABLE)
    set(DXC_EXECUTABLE "${CMAKE_SOURCE_DIR}/shaders/dxc/dxc.exe")
    
    if(NOT EXISTS ${DXC_EXECUTABLE})
        message(FATAL_ERROR "Dxc compiler not found! Please install via VCPKG or place dxc.exe in ${CMAKE_SOURCE_DIR}/shaders/dxc/")
    endif()
endif()

set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADERS_CACHE_DIR "${SHADERS_DIR}/cache")

file(MAKE_DIRECTORY ${SHADERS_CACHE_DIR})

add_subdirectory(src/modules/core)
add_subdirectory(src/modules/renderer)
add_subdirectory(src/modules/input)
add_subdirectory(src/modules/logger)
add_subdirectory(shaders)

file(GLOB IMGUI_SOURCES
    "${CMAKE_SOURCE_DIR}/include/imgui/*.cpp"
    "${CMAKE_SOURCE_DIR}/include/imgui/vulkan/*.cpp"
)

add_library(EngineImgui STATIC ${IMGUI_SOURCES})

target_compile_options(EngineImgui PUBLIC /utf-8)

target_include_directories(EngineImgui PUBLIC
    "${CMAKE_SOURCE_DIR}/include/imgui"
    "${CMAKE_SOURCE_DIR}/include/imgui/vulkan"
    "${CMAKE_SOURCE_DIR}/include/glfw/include"
    "${CMAKE_SOURCE_DIR}/include/glm"
    "${CMAKE_SOURCE_DIR}/include/assimp/include"
    ${Vulkan_INCLUDE_DIRS}
)

add_subdirectory(src/engine)
add_subdirectory(src/game)
add_subdirectory(src/editor)
add_subdirectory(src/game_app)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Editor)