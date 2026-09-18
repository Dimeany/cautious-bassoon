if("ADCS" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})

    add_executable(test_cube_space_driver
        test_cube_space_driver.cpp
    )

    # Include the include directory
    target_include_directories(test_cube_space_driver
        PRIVATE
        "${COMMON_LIBRARY_DIRECTORY_PATH}/include"
    )

    target_link_libraries(
        test_cube_space_driver
        adcs
        i2c
        gtest
        pthread
    )

    gtest_discover_tests(test_cube_space_driver)

    add_executable(TablesJsonTest
        test_command_sender.cpp
    )
    target_link_libraries(
        TablesJsonTest
        CubeSpaceTablesJson
        gtest
        pthread
    )
    gtest_discover_tests(TablesJsonTest)
endif()
