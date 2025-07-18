add_library(logger INTERFACE)

target_include_directories(logger INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/spdlog/include
)

# target_link_libraries(logger INTERFACE spdlog::spdlog)