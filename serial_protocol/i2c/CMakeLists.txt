add_library(i2c
    i2c_setting.cpp
    i2c.cpp
)

# Include the include directory
target_include_directories(i2c
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)