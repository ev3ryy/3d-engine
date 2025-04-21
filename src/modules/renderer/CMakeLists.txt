file(GLOB_RECURSE RENDERER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RENDERER_SOURCES})

find_package(Vulkan REQUIRED)
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};C:/Users/Ev3ry/vcpkg/installed/x64-windows/share/VulkanMemoryAllocator")
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

add_library(renderer ${RENDERER_SOURCES})

target_include_directories(renderer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/spdlog/include
    ${CMAKE_SOURCE_DIR}/include/glfw/include
    "${CMAKE_SOURCE_DIR}/include/imgui"
    "${CMAKE_SOURCE_DIR}/include/imgui/vulkan"
    "${CMAKE_SOURCE_DIR}/include/imgui/backends"
)

target_link_libraries(renderer PUBLIC
    glfw
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
)