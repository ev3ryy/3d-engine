file(GLOB_RECURSE RENDERER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RENDERER_SOURCES})

add_library(logger ${RENDERER_SOURCES})

target_include_directories(renderer PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_SOURCE_DIR}/include/spdlog/include
)
