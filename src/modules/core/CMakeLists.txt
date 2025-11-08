file(GLOB_RECURSE CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CORE_SOURCES})

add_library(core ${CORE_SOURCES})

if(MSVC)
    target_compile_options(core PUBLIC /utf-8)
else()
    target_compile_options(core PUBLIC -Wall -Wextra -Wpedantic)
endif()

# target_include_directories(core PUBLIC
#     ${CMAKE_SOURCE_DIR}/include/JoltPhysics/Jolt
# )

target_link_libraries(core PUBLIC 
    logger
)