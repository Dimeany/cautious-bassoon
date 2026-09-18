if("EPS" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)


# EPS Driver
add_library(
    eps_driver
        csp_driver.c
        csp_functions.c
        csp_parameters.c
        struct_decode_operators
)
target_link_libraries(
    eps_driver
    PUBLIC
        SpaceInventor_param
        SpaceInventor_csp
)
target_compile_options(
    eps_driver
    PRIVATE
        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-sign-compare"
)


# Demonstration and test bed of the driver
add_executable(
    eps_driver_demo
        eps_driver_demo.cpp
)
target_link_libraries(
    eps_driver_demo
    PUBLIC
        eps_driver
)


# Test EPS Driver
add_executable(
    test_eps_driver
        test_eps_driver.cpp
)
target_link_libraries(
    test_eps_driver
    PUBLIC
        eps_driver
        socket
)


# EPS Service
add_executable(
    eps_service
        eps_service.cpp
        struct_decode_operators.cpp
)
target_link_libraries(
    eps_service
    PUBLIC
        eps_driver
        socket
        project_utils
)


# CSP Example Executable
add_executable(
    csp_example
        csp_example.c
)
target_link_libraries(
    csp_example
    PUBLIC
        SpaceInventor_param
        SpaceInventor_csp
)

# No libparam version of the example
# add_library(
#     mpack
#         ../vendor/SpaceInventor/libparam/src/mpack/mpack.c
# )
# target_include_directories(mpack
#     PUBLIC
#         # The directory containing the 'mpack' folder
#         ../vendor/SpaceInventor/Nebula-meson-compatibility

# )
# add_executable(
#     csp_example_nolibparam
#     csp_example_nolibparam.c
# )
# target_link_libraries(
#     csp_example_nolibparam
#     PUBLIC
#         SpaceInventor_csp
#         mpack
# )
# target_compile_options(
#     csp_example_nolibparam
#     PRIVATE
#         "-Wno-unused-function"
#         "-Wno-unused-parameter"
#         "-Wno-sign-compare"
# )


# EPS decode operators
add_library(eps
    struct_decode_operators.cpp
)

# Include the include directory
target_include_directories(eps
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(
    TARGETS 
        eps_service 
        eps_driver_demo 
        test_eps_driver 
        csp_example
    COMPONENT binaries
)
endif()