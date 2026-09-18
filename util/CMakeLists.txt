add_library(project_utils STATIC
    environment_util.cpp
    status_util.cpp
)

target_include_directories(project_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})