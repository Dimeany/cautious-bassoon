install(
    DIRECTORY bin/
    DESTINATION bin
    FILE_PERMISSIONS
        OWNER_READ OWNER_EXECUTE OWNER_WRITE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
    COMPONENT binaries
)
install(
    DIRECTORY etc/ DESTINATION /etc
    COMPONENT configuration
)
install(
    FILES boot/config.txt DESTINATION /boot/firmware
    COMPONENT configuration
)
install(
    FILES lib/udev/hwclock-set DESTINATION /usr/lib/udev
    COMPONENT configuration
)
