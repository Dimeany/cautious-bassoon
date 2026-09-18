find_package(PkgConfig REQUIRED)
pkg_check_modules(BSD REQUIRED IMPORTED_TARGET libbsd)
pkg_check_modules(YAML REQUIRED IMPORTED_TARGET yaml-0.1)
pkg_check_modules(SOCKETCAN REQUIRED IMPORTED_TARGET libsocketcan)

add_library(
    SpaceInventor_csp
        # from libcsp/src/meson.build
        libcsp/src/csp_rdp.c
        libcsp/src/csp_rdp_queue.c
        libcsp/src/csp_buffer.c
        libcsp/src/csp_bridge.c
        libcsp/src/csp_conn.c
        libcsp/src/csp_crc32.c
        libcsp/src/csp_debug.c
        libcsp/src/csp_dedup.c
        libcsp/src/csp_hex_dump.c
        libcsp/src/csp_iflist.c
        libcsp/src/csp_init.c
        libcsp/src/csp_io.c
        libcsp/src/csp_port.c
        libcsp/src/csp_promisc.c
        libcsp/src/csp_qfifo.c
        libcsp/src/csp_route.c
        libcsp/src/csp_rtable_cidr.c
        libcsp/src/csp_service_handler.c
        libcsp/src/csp_services.c
        libcsp/src/csp_id.c
        libcsp/src/csp_sfp.c

        # from libcsp/src/meson.build (yaml found)
        libcsp/src/csp_yaml.c

        # from libcsp/src/arch/posix/meson.build
        libcsp/src/arch/posix/csp_queue.c
        libcsp/src/arch/posix/csp_semaphore.c
        libcsp/src/arch/posix/csp_system.c
        libcsp/src/arch/posix/csp_time.c
        libcsp/src/arch/posix/csp_clock.c
        libcsp/src/arch/posix/pthread_queue.c

        # from libcsp/src/crypto/meson.build
        libcsp/src/crypto/csp_hmac.c
        libcsp/src/crypto/csp_sha1.c

        # from libcsp/src/drivers/meson.build
        libcsp/src/drivers/can/can_socketcan.c
        libcsp/src/drivers/usart/usart_linux.c
        libcsp/src/drivers/usart/usart_kiss.c

        # from libcsp/src/interfaces/meson.build
        libcsp/src/interfaces/csp_if_lo.c
        libcsp/src/interfaces/csp_if_can.c
        libcsp/src/interfaces/csp_if_can_pbuf.c
        libcsp/src/interfaces/csp_if_kiss.c
        libcsp/src/interfaces/csp_if_tun.c
        libcsp/src/interfaces/csp_if_i2c.c

        # from libcsp/src/interfaces/meson.build (sys/socket.h found)
        libcsp/src/interfaces/csp_if_udp.c
)
target_link_libraries(
    SpaceInventor_csp
    PUBLIC
        PkgConfig::SOCKETCAN
        PkgConfig::YAML
)
target_include_directories(
    SpaceInventor_csp
    SYSTEM PUBLIC
        libcsp/include
        Nebula-meson-compatibility
)
set_target_properties(
    SpaceInventor_csp
    PROPERTIES
        COMPILE_WARNING_AS_ERROR OFF
)
target_compile_options(
    SpaceInventor_csp
    PRIVATE
        "-w"
)

add_library(
    SpaceInventor_param
        libparam/src/param/list/param_list.c
        libparam/src/param/param_client.c
        libparam/src/param/param_serializer.c
        libparam/src/param/param_server.c
        libparam/src/param/param_string.c
        libparam/src/param/param_queue.c
        libparam/src/param/param_wildcard.c
        libparam/src/param
        libparam/src/param/param.c
        libparam/src/mpack/mpack.c

        libparam/src/vmem/vmem_client.c
        libparam/src/vmem/vmem_server.c
        libparam/src/vmem/vmem.c
        libparam/src/vmem/vmem_block.c
        libparam/src/vmem/vmem_client.c
        libparam/src/vmem/vmem_crc32.c
        libparam/src/vmem/vmem_file.c
        libparam/src/vmem/vmem_fram.c
        libparam/src/vmem/vmem_fram_cache.c
        libparam/src/vmem/vmem_mmap.c

        libparam/src/objstore/objstore.c
)
target_include_directories(
    SpaceInventor_param
    SYSTEM PUBLIC
        libparam/include
)
target_link_libraries(
    SpaceInventor_param
    PUBLIC
        PkgConfig::BSD
        SpaceInventor_csp
)
set_target_properties(
    SpaceInventor_param
    PROPERTIES
        COMPILE_WARNING_AS_ERROR OFF
)
target_compile_options(
    SpaceInventor_param
    PRIVATE
        "-w"
)
