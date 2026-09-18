# The service has the main code logic;
# we just ovveride how the health packet is created
# in order to add the Smallsat header onto it
add_library(smallsat-det-service STATIC)

target_sources(
    smallsat-det-service
    PRIVATE
        SmallsatDetService.hh
)

target_include_directories(
    smallsat-det-service
    PUBLIC
        .
)

target_link_libraries(
    smallsat-det-service
    PUBLIC
    	atomic
        hafx-ctrl
        x123-ctrl
	    pthread
        det-service

        # Finally, in this case,
        # we want to link to the smallsat UDP
        # libraries so that we can add on the
        # custom header for this project
        udp
)


# main executable
add_executable(det-controller main.cc)

# need built .a to be available
target_link_directories(
    det-controller PUBLIC "${LIB_OUT_DIR}")

target_link_libraries(
    det-controller
    PUBLIC
        smallsat-det-service
        det-support
        logging
)

# Boost
find_package(Boost ${BOOSTVERSION} REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
target_link_directories(det-controller PUBLIC ${Boost_LIBRARY_DIRS})

target_sources(
    det-controller
    PRIVATE
        "../controller-code/det-controller/Listener.cc"
)
target_include_directories(
    det-controller
    PUBLIC
        .
        "../controller-code/det-controller/"
)
