cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0057 NEW)

if("COMMS" IN_LIST SMALLSAT_SUBSYSTEMS_LIST)
    # Define the COMMS service executable
    add_executable(comms_service
        comms_service.cpp
        file_descriptor_manager.cpp
        backends/ethernet.cpp
        backends/filein.cpp
        backends/fileout.cpp
        backends/ppp.cpp
        backends/serial.cpp
    )

    # Include directories for the COMMS service executable
    target_include_directories(comms_service
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # Link libraries for the COMMS service executable
    target_link_libraries(comms_service
        socket
        udp
    )

    # Install the COMMS service executable
    install(
        TARGETS comms_service
        COMPONENT binaries
    )

    # ---------------------------
    # Define the EXACT COMMS service executable
    # Build the standalone BFT executable in this directory
    add_executable(comms_bft_recv
        bft_recv.cpp
        file_descriptor_manager.cpp
        backends/ccsds.cpp
    )

    target_include_directories(comms_bft_recv
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_include_directories(comms_bft_recv PRIVATE ${SYSTEMD_INCLUDE_DIRS})

    target_link_libraries(comms_bft_recv
        socket
        udp
        ${SYSTEMD_LIBRARIES}
    )

    install(
        TARGETS comms_bft_recv
        COMPONENT binaries
    )

    # Define the EXACT COMMS service executable
    add_executable(exact_comms_service
        exact_comms_service.cpp
        file_descriptor_manager.cpp
        backends/ccsds.cpp
        backends/serial.cpp
    )

    # Include directories for the EXACT COMMS service executable
    target_include_directories(exact_comms_service
        PUBLIC    
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_compile_features(exact_comms_service PRIVATE cxx_std_20)

    # Link libraries for the EXACT COMMS service executable
    target_link_libraries(exact_comms_service
        socket
        udp    
    )
    # Ensure the BFT executable is built whenever exact_comms_service is built
    add_dependencies(exact_comms_service comms_bft_recv)

    install(
        TARGETS exact_comms_service
        COMPONENT binaries
    )
    
    # Stripping is handled globally by the top-level STRIP_BINARIES option.
endif()
