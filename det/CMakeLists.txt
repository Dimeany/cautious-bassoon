# Symlink to the 'controller-code' directory to shorten the
# lines here
# Sorry it's silly :p

# Order matters (dependencies)
add_subdirectory(controller-code/logging)
add_subdirectory(controller-code/det-messages)
add_subdirectory(controller-code/det-support)

add_subdirectory(controller-code/libusb-cpp)
add_subdirectory(controller-code/sipm3k-interface)
add_subdirectory(controller-code/x123-interface)

add_subdirectory(controller-code/hafx-ctrl)
add_subdirectory(controller-code/x123-ctrl)

add_subdirectory(controller-code/det-service)

# Customize this for IMPRESS/EXACT where the
# health packets get headers attached to them
add_subdirectory(smallsat-det-controller)

# Required for PPS edge detection
add_subdirectory(controller-code/edge-detector)

# Install target
install(
    # baseline programs required to function
    TARGETS det-controller
    COMPONENT binaries
)

# All write/read
file(
    CHMOD "$ENV{HOME}/detector-config"
    PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_WRITE GROUP_EXECUTE
        WORLD_READ WORLD_WRITE WORLD_EXECUTE
)
