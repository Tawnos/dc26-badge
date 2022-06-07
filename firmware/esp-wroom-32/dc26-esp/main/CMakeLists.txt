cmake_minimum_required(VERSION 3.10)

project(darknet7-esp-main)

set(CMAKE_CXX_STANDARD 20)

# configure_file(../sdkconfig sdkconfig.h)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set( INCLUDEDIRS
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/esp32/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/soc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/soc/esp32/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/driver/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/esp_http_client/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/esp_http_client/lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/freertos/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/heap/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/json/cJSON
    ${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/nghttp/nghttp2/third-party/http-parser

    #${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/newlib/include
    #${CMAKE_CURRENT_SOURCE_DIR}/../../esp-idf/components/newlib/platform_include

    ${CMAKE_CURRENT_SOURCE_DIR}/../components/libcrc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/flatbuffer/
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/mkfatfs/src/idf/modified/driver/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../components/mkfatfs/src/idf/modified/log/include

)

set ( COMPILEDEFS
    FLATBUFFERS_TRACK_VERIFIER_BUFFER_SIZE
    CONFIG_CXX_EXCEPTIONS=1
    __attribute__\(x\)=__declspec\(\(x\)\)
    VIRTUAL_DEVICE
)

set (SOURCEFILES
    command_handler.cpp
    command_handler.h
    command_queue.h
    # common_generated.h
    # dc26.cpp
    # dc26.h
    display_handler.cpp
    display_handler.h
    mcu_to_mcu.cpp
    mcu_to_mcu.h
    ../sdkconfig.h
    npc_interact.cpp
    npc_interact.h
    esp_to_stm_generated.h 
    stm_to_esp_generated.h 
    task_handler.h

    ../components/flatbuffer/flatbuffers/flatbuffers.h
    ../components/libcrc/include/checksum.h
    ../../esp-idf/components/esp_http_client/include/esp_http_client.h
    ../../esp-idf/components/esp32/include/esp_system.h
    ../../esp-idf/components/json/cJSON/cJSON.h
    ../../esp-idf/components/log/include/esp_log.h
    ../../esp-idf/components/freertos/include/freertos/queue.h

    # dc26_ble/ble_serial.cpp    
    # dc26_ble/ble_serial.h
    # dc26_ble/ble.cpp
    # dc26_ble/ble.h
    # dc26_ble/pairing.cpp
    # dc26_ble/pairing.h
    # dc26_ble/scanning.cpp
    # dc26_ble/scanning.h
    # dc26_ble/security.cpp
    # dc26_ble/security.h
    # dc26_ble/services.h

    # lib/FATFS_VFS.cpp   
    # lib/FATFS_VFS.h     
    # lib/File.cpp        
    # lib/File.h          
    # lib/FileSystem.cpp  
    # lib/FileSystem.h    
    # lib/fonts.cpp       
    # lib/fonts.h         
    # lib/FreeRTOS.cpp    
    # lib/FreeRTOS.h      
    # lib/GeneralUtils.cpp
    # lib/GeneralUtils.h  
    lib/gui.cpp         
    lib/gui.h           
    # lib/i2c.cpp         
    lib/i2c.hpp         
    # lib/Memory.cpp      
    # lib/Memory.h        
    # lib/ssd1306.cpp     
    lib/ssd1306.h       
    # lib/System.cpp      
    lib/System.h        
    # lib/Task.cpp        
    # lib/Task.h   
    
    # lib/ble/BLE2902.cpp
    # lib/ble/BLE2902.h
    # lib/ble/BLE2904.cpp
    # lib/ble/BLE2904.h
    # lib/ble/BLEAddress.cpp
    # lib/ble/BLEAddress.h
    # lib/ble/BLEAdvertisedDevice.cpp
    # lib/ble/BLEAdvertisedDevice.h
    # lib/ble/BLEAdvertising.cpp
    # lib/ble/BLEAdvertising.h
    # lib/ble/BLEBeacon.cpp
    # lib/ble/BLEBeacon.h
    # lib/ble/BLECharacteristic.cpp
    # lib/ble/BLECharacteristic.h
    # lib/ble/BLECharacteristicMap.cpp
    # lib/ble/BLEClient.cpp
    # lib/ble/BLEClient.h
    # lib/ble/BLEDescriptor.cpp
    # lib/ble/BLEDescriptor.h
    # lib/ble/BLEDescriptorMap.cpp
    # lib/ble/BLEDevice.cpp
    # lib/ble/BLEDevice.h
    # lib/ble/BLEExceptions.cpp
    # lib/ble/BLEExceptions.h
    # lib/ble/BLEHIDDevice.cpp
    # lib/ble/BLEHIDDevice.h
    # lib/ble/BLERemoteCharacteristic.cpp
    # lib/ble/BLERemoteCharacteristic.h
    # lib/ble/BLERemoteDescriptor.cpp
    # lib/ble/BLERemoteDescriptor.h
    # lib/ble/BLERemoteService.cpp
    # lib/ble/BLERemoteService.h
    # lib/ble/BLEScan.cpp
    # lib/ble/BLEScan.h
    # lib/ble/BLESecurity.cpp
    # lib/ble/BLESecurity.h
    # lib/ble/BLEServer.cpp
    # lib/ble/BLEServer.h
    # lib/ble/BLEService.cpp
    # lib/ble/BLEService.h
    # lib/ble/BLEServiceMap.cpp
    # lib/ble/BLEUtils.cpp
    # lib/ble/BLEUtils.h
    # lib/ble/BLEUUID.cpp
    # lib/ble/BLEUUID.h
    # lib/ble/BLEValue.cpp
    # lib/ble/BLEValue.h
    # lib/ble/HIDTypes.h

    lib/json/JSON.h
    lib/json/JSON.cpp

    # lib/net/GeneralUtils.cpp
    # lib/net/GeneralUtils.h
    # lib/net/HttpParser.cpp
    # lib/net/HttpParser.h
    # lib/net/HttpRequest.cpp
    # lib/net/HttpRequest.h
    # lib/net/HttpResponse.cpp
    # lib/net/HttpResponse.h
    # lib/net/HttpServer.cpp
    # lib/net/HttpServer.h
    # lib/net/Socket.cpp
    # lib/net/Socket.h
    # lib/net/SockServ.cpp
    # lib/net/SockServ.h
    # lib/net/SSLUtils.cpp
    # lib/net/SSLUtils.h
    # lib/net/WebSocket.cpp
    # lib/net/WebSocket.h
    # lib/net/WebSocketFileTransfer.cpp
    # lib/net/WebSocketFileTransfer.h

    # lib/wifi/WiFi.cpp
    # lib/wifi/WiFi.h
    # lib/wifi/WiFiEventHandler.cpp
    # lib/wifi/WiFiEventHandler.h
)

add_library(darknet7-esp-main STATIC ${SOURCEFILES})

target_compile_definitions(darknet7-esp-main PUBLIC ${COMPILEDEFS})
target_include_directories(darknet7-esp-main PUBLIC ${INCLUDEDIRS})
