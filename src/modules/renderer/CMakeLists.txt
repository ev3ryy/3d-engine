file(GLOB_RECURSE RENDERER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    # Add other renderer subdirectories as needed
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RENDERER_SOURCES})

find_package(Vulkan REQUIRED)

add_library(renderer ${RENDERER_SOURCES})

add_dependencies(renderer CompileShaders)

target_compile_options(renderer PUBLIC /utf-8)

target_include_directories(renderer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}

    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/engine/scene>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/engine/scene/components>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/modules/core>  

    #${CMAKE_SOURCE_DIR}/include/spdlog/include
    ${CMAKE_SOURCE_DIR}/include/glfw/include
    ${CMAKE_SOURCE_DIR}/include/glm
    "${CMAKE_SOURCE_DIR}/include/imgui"
    "${CMAKE_SOURCE_DIR}/include/imgui/vulkan"
    "${CMAKE_SOURCE_DIR}/include/imgui/backends"

    ${CMAKE_SOURCE_DIR}/include/VulkanMemoryAllocator/include

    ${CMAKE_SOURCE_DIR}/include/bullet3/src

    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(renderer PUBLIC
    core
    glfw
    logger
    Vulkan::Vulkan
    BulletDynamics
)