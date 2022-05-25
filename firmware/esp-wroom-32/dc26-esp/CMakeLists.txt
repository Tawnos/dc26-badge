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
    ${CMAKE_CURRENT_SOURCE_DIR}/../esp-idf/components/newlib/include
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
    main/command_handler.cpp
    main/command_handler.h
    main/common_generated.h
    main/dc26.cpp
    main/dc26.h
    main/display_handler.cpp
    main/display_handler.h
    main/esp_to_stm_generated.h 
    main/mcu_to_mcu.cpp
    main/mcu_to_mcu.h
    main/npc_interact.cpp
    main/npc_interact.h
    main/stm_to_esp_generated.h 

    main/dc26_ble/ble_serial.cpp    
    main/dc26_ble/ble_serial.h
    main/dc26_ble/ble.cpp
    main/dc26_ble/ble.h
    main/dc26_ble/pairing.cpp
    main/dc26_ble/pairing.h
    main/dc26_ble/scanning.cpp
    main/dc26_ble/scanning.h
    main/dc26_ble/security.cpp
    main/dc26_ble/security.h
    main/dc26_ble/services.h

    main/lib/FATFS_VFS.cpp   
    main/lib/FATFS_VFS.h     
    main/lib/File.cpp        
    main/lib/File.h          
    main/lib/FileSystem.cpp  
    main/lib/FileSystem.h    
    main/lib/fonts.cpp       
    main/lib/fonts.h         
    main/lib/FreeRTOS.cpp    
    main/lib/FreeRTOS.h      
    main/lib/GeneralUtils.cpp
    main/lib/GeneralUtils.h  
    main/lib/gui.cpp         
    main/lib/gui.h           
    main/lib/i2c.cpp         
    main/lib/i2c.hpp         
    main/lib/Memory.cpp      
    main/lib/Memory.h        
    main/lib/ssd1306.cpp     
    main/lib/ssd1306.h       
    main/lib/System.cpp      
    main/lib/System.h        
    main/lib/Task.cpp        
    main/lib/Task.h   
    
    main/lib/ble/BLE2902.cpp
    main/lib/ble/BLE2902.h
    main/lib/ble/BLE2904.cpp
    main/lib/ble/BLE2904.h
    main/lib/ble/BLEAddress.cpp
    main/lib/ble/BLEAddress.h
    main/lib/ble/BLEAdvertisedDevice.cpp
    main/lib/ble/BLEAdvertisedDevice.h
    main/lib/ble/BLEAdvertising.cpp
    main/lib/ble/BLEAdvertising.h
    main/lib/ble/BLEBeacon.cpp
    main/lib/ble/BLEBeacon.h
    main/lib/ble/BLECharacteristic.cpp
    main/lib/ble/BLECharacteristic.h
    main/lib/ble/BLECharacteristicMap.cpp
    main/lib/ble/BLEClient.cpp
    main/lib/ble/BLEClient.h
    main/lib/ble/BLEDescriptor.cpp
    main/lib/ble/BLEDescriptor.h
    main/lib/ble/BLEDescriptorMap.cpp
    main/lib/ble/BLEDevice.cpp
    main/lib/ble/BLEDevice.h
    main/lib/ble/BLEExceptions.cpp
    main/lib/ble/BLEExceptions.h
    main/lib/ble/BLEHIDDevice.cpp
    main/lib/ble/BLEHIDDevice.h
    main/lib/ble/BLERemoteCharacteristic.cpp
    main/lib/ble/BLERemoteCharacteristic.h
    main/lib/ble/BLERemoteDescriptor.cpp
    main/lib/ble/BLERemoteDescriptor.h
    main/lib/ble/BLERemoteService.cpp
    main/lib/ble/BLERemoteService.h
    main/lib/ble/BLEScan.cpp
    main/lib/ble/BLEScan.h
    main/lib/ble/BLESecurity.cpp
    main/lib/ble/BLESecurity.h
    main/lib/ble/BLEServer.cpp
    main/lib/ble/BLEServer.h
    main/lib/ble/BLEService.cpp
    main/lib/ble/BLEService.h
    main/lib/ble/BLEServiceMap.cpp
    main/lib/ble/BLEUtils.cpp
    main/lib/ble/BLEUtils.h
    main/lib/ble/BLEUUID.cpp
    main/lib/ble/BLEUUID.h
    main/lib/ble/BLEValue.cpp
    main/lib/ble/BLEValue.h
    main/lib/ble/HIDTypes.h

    main/lib/json/JSON.h
    main/lib/json/JSON.cpp

    main/lib/net/GeneralUtils.cpp
    main/lib/net/GeneralUtils.h
    main/lib/net/HttpParser.cpp
    main/lib/net/HttpParser.h
    main/lib/net/HttpRequest.cpp
    main/lib/net/HttpRequest.h
    main/lib/net/HttpResponse.cpp
    main/lib/net/HttpResponse.h
    main/lib/net/HttpServer.cpp
    main/lib/net/HttpServer.h
    main/lib/net/Socket.cpp
    main/lib/net/Socket.h
    main/lib/net/SockServ.cpp
    main/lib/net/SockServ.h
    main/lib/net/SSLUtils.cpp
    main/lib/net/SSLUtils.h
    main/lib/net/WebSocket.cpp
    main/lib/net/WebSocket.h
    main/lib/net/WebSocketFileTransfer.cpp
    main/lib/net/WebSocketFileTransfer.h

    main/lib/wifi/WiFi.cpp
    main/lib/wifi/WiFi.h
    main/lib/wifi/WiFiEventHandler.cpp
    main/lib/wifi/WiFiEventHandler.h
)

add_library(darknet7-esp STATIC ${SOURCEFILES})

target_compile_definitions(darknet7-esp PUBLIC ${COMPILEDEFS})
target_include_directories(darknet7-esp PUBLIC ${INCLUDEDIRS})
