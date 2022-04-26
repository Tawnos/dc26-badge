cmake_minimum_required(VERSION 3.10)

project(darknet7)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set( INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/3dParty/flatbuffer
    ${CMAKE_CURRENT_SOURCE_DIR}/libstm32
    ${CMAKE_CURRENT_SOURCE_DIR}/libstm32/display
    ${CMAKE_CURRENT_SOURCE_DIR}/libstm32/etl/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Include
)

set ( COMPILEDEFS
    VIRTUAL_DEVICE
    #STM32F411xE 
    __weak=__attribute__\(\(weak\)\)
    __packed=__attribute__\(\(__packed__\)\)
    #USE_HAL_DRIVER 
    FLATBUFFERS_TRACK_VERIFIER_BUFFER_SIZE 
)

set (SOURCEFILES
    #darknet7.cpp
    common.h
    contact.h
    darknet7.h
    #button_info.cpp
    button_info.h
    #hardware_config.cpp  
    init.cpp             
    init.h
    contact_store.cpp         
    contact_store.h         
    #mcu_to_mcu.cpp      
    my_info.h 
    settings_info.h
    virtual_key_board.cpp
    virtual_key_board.h

    3dParty/flatbuffer/flatbuffers/flatbuffers.h
    libstm32/rgbcolor.cpp
    libstm32/rgbcolor.h

    libstm32/app/app.h
    libstm32/app/display_message_state.cpp
    libstm32/app/display_message_state.h
    libstm32/app/state_base.cpp
    libstm32/app/state_base.h

    libstm32/display/display_device.cpp
    libstm32/display/display_device.h
    libstm32/display/fonts.cpp
    libstm32/display/fonts.h
    libstm32/display/gui.cpp
    libstm32/display/gui.h

    libstm32/crypto/crypto_helper.cpp
    libstm32/crypto/crypto_helper.h
    libstm32/crypto/sha256.cpp
    libstm32/crypto/sha256.h
    libstm32/crypto/micro-ecc/uECC.c
    libstm32/crypto/micro-ecc/uECC.h

    libstm32/utility/bitarray.cpp
    libstm32/utility/bitarray.h

    art/images.h
    art/tits.cpp
    art/stickers.cpp
    art/dc_zia_96-sized.cpp
    art/wetriffs.cpp

    menus/AddressState.cpp           
    menus/AddressState.h           
    menus/badge_info_state.cpp       
    menus/badge_info_state.h       
    menus/communications_settings.cpp
    menus/communications_settings.h
    #menus/GameOfLife.cpp             
    menus/gui_list_processor.cpp     
    menus/gui_list_processor.h     
    menus/health.cpp                 
    menus/health.h                 
    #menus/mcu_info.cpp               
    menus/menu_state.cpp             
    menus/menu_state.h             
    #menus/menu3d.cpp                 
    #menus/pairing_state.cpp          
    #menus/sao_menu.cpp               
    #menus/scan.cpp                   
    menus/SendMsgState.cpp           
    menus/SendMsgState.h           
    menus/setting_state.cpp          
    menus/setting_state.h          
    #menus/tamagotchi.cpp             
    #menus/test_state.cpp         

    #menus/3d/renderer.cpp
    #menus/3d/vec_math.cpp

    messaging/common_generated.h
    messaging/esp_to_stm_generated.h
    messaging/stm_to_esp_generated.h
)

add_library(darknet7 STATIC ${SOURCEFILES})

target_compile_definitions(darknet7 PUBLIC ${COMPILEDEFS})
target_include_directories(darknet7 PUBLIC ${INCLUDEDIRS})
