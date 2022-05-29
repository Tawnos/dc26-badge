cmake_minimum_required(VERSION 3.10)

project(darknet7-esp)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set( INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/components/flatbuffer/
    ${CMAKE_CURRENT_SOURCE_DIR}/components/libcrc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/components/mkfatfs/src/idf/modified/log/include
    ${CMAKE_CURRENT_SOURCE_DIR}/components/mkfatfs/src/idf/modified/freertos/include
    ${CMAKE_CURRENT_SOURCE_DIR}/main
    ${CMAKE_CURRENT_SOURCE_DIR}/main/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/esp32/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/driver/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/freertos/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/heap/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/lwip/include/lwip
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/lwip/include/lwip/port
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/mbedtls/mbedtls/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/mdns/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/newlib/includeb
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/newlib/platform_include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/nvs_flash/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/soc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/soc/esp32/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/tcpip_adapter/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/spi_flash/sim
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/vfs/include

)

set ( COMPILEDEFS
    FLATBUFFERS_TRACK_VERIFIER_BUFFER_SIZE
    __attribute__\(x\)=__declspec\(\(x\)\)
)

set (SOURCEFILES
    main/dc26.cpp
    main/dc26.h
)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/main)


add_library(darknet7-esp STATIC ${SOURCEFILES})

target_compile_definitions(darknet7-esp PUBLIC ${COMPILEDEFS})
target_include_directories(darknet7-esp PUBLIC ${INCLUDEDIRS})
