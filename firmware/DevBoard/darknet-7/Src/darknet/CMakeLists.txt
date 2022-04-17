cmake_minimum_required(VERSION 3.10)

project(darknet7)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set( INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../../Inc
    3dParty/flatbuffer
    libstm32
    libstm32/display
    libstm32/etl/include
    ../../Drivers/STM32F4xx_HAL_Driver/Inc
    ../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ../../Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ../../Drivers/CMSIS/Include
)

set ( COMPILEDEFS
    VIRTUAL_DEVICE
    #STM32F411xE 
    __weak=__attribute__\(\(weak\)\)
    __packed=__attribute__\(\(__packed__\)\)
    #USE_HAL_DRIVER 
    FLATBUFFERS_TRACK_VERIFIER_BUFFER_SIZE 
)
add_compile_definitions(${COMPILEDEFS})

add_library(darknet7 STATIC 
    darknet7.cpp
    #button_info.cpp
    #hardware_config.cpp  
    init.cpp             
    KeyStore.cpp         
    #mcu_to_mcu.cpp       
    #virtual_key_board.cpp

    libstm32/display/display_device.cpp
    libstm32/display/fonts.cpp
    libstm32/display/gui.cpp

    art/tits.cpp
    art/stickers.cpp
    art/dc_zia_96-sized.cpp
    art/wetriffs.cpp

    menus/AddressState.cpp           
    menus/badge_info_state.cpp       
    menus/communications_settings.cpp
    #menus/GameOfLife.cpp             
    menus/gui_list_processor.cpp     
    menus/health.cpp                 
    #menus/mcu_info.cpp               
    menus/menu_state.cpp             
    #menus/menu3d.cpp                 
    #menus/pairing_state.cpp          
    #menus/sao_menu.cpp               
    #menus/scan.cpp                   
    #menus/SendMsgState.cpp           
    menus/setting_state.cpp          
    menus/tamagotchi.cpp             
    #menus/test_state.cpp         

    #menus/3d/renderer.cpp
    #menus/3d/vec_math.cpp
)

target_compile_definitions(darknet7 INTERFACE ${COMPILEDEFS})
target_include_directories(darknet7 INTERFACE ${INCLUDEDIRS})
include_directories(${INCLUDEDIRS})