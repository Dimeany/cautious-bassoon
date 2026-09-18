add_library(
    novatel
    novatel_edie/src/decoders/common/src/circularbuffer.cpp
    novatel_edie/src/decoders/common/src/common.cpp
    novatel_edie/src/decoders/common/src/jsonreader.cpp
    novatel_edie/src/decoders/common/src/message_decoder.cpp
    novatel_edie/src/decoders/novatel/src/encoder.cpp
    novatel_edie/src/decoders/novatel/src/framer.cpp
    novatel_edie/src/decoders/novatel/src/header_decoder.cpp
)

target_include_directories(
    novatel
    SYSTEM PUBLIC
    novatel_edie/src
    novatel_edie/src/decoders/common/api
    novatel_edie/src/decoders/novatel/api
    novatel_edie/lib
    additional_novatel_headers
)

set_target_properties(
    novatel
    PROPERTIES
        COMPILE_WARNING_AS_ERROR OFF
)

target_compile_options(
    novatel
    PRIVATE
        "-w"
)

install(
    FILES novatel_edie/database/messages_public.json
    DESTINATION /etc/SMALLSAT/novatel
)
