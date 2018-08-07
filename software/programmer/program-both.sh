./program-normal-user.sh
python $HOME/dev/defcon/dc26-badge/firmware/esp-wroom-32/esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0x1000 $HOME/dev/defcon/dc26-badge/firmware/esp-wroom-32/dc26-esp/build/bootloader/bootloader.bin 0x10000 $HOME/dev/defcon/dc26-badge/firmware/esp-wroom-32/dc26-esp/build/dc26esp.bin 0x8000 $HOME/dev/defcon/dc26-badge/firmware/esp-wroom-32/dc26-esp/build/partitions.bin
python $HOME/dev/defcon/dc26-badge/firmware/esp-wroom-32/esp-idf/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0x210000 $HOME/dev/defcon/dc26-badge/firmware/esp-wroom-32/dc26-esp/build/fatfs_image.img
