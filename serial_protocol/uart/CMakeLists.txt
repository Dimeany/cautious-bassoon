add_library(uart
    uart.cpp
)

# Include the include directory
target_include_directories(uart
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
