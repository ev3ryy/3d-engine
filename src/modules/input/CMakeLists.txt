file(GLOB_RECURSE INPUT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${INPUT_SOURCES})

add_library(input ${INPUT_SOURCES})

target_include_directories(input PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_SOURCE_DIR}/include/spdlog/include
    ${CMAKE_SOURCE_DIR}/include/glfw/include
    ${CMAKE_SOURCE_DIR}/include/imgui
    ${CMAKE_SOURCE_DIR}/include/imgui/vulkan
    )
