
#include <esp_system.h>
#include <esp_log.h>
#include <driver/uart.h>
#include <driver/gpio.h>
#include <soc/uart_struct.h>
#include <nvs_flash.h>
#include <string>
#include "stm_to_esp_generated.h"
#include "esp_to_stm_generated.h"
#include "lib/System.h"
#include "lib/ssd1306.h"
#include "mcu_to_mcu.h"
#include "command_handler.h"
#include "display_handler.h"

#include "dc26_ble/ble.h"

#if defined USE_FATFS
#include "lib/FATFS_VFS.h"
#endif

static const int RX_BUF_SIZE = 1024;
#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)

extern "C" {
		void app_main(void);
		static void generalCmdTask(void *);
}

#if defined USE_FATFS
FATFS_VFS *FatFS = new FATFS_VFS("/spiflash", "storage");
#endif
static xQueueHandle gpio_evt_queue = NULL;
CmdHandlerTask CmdTask("CmdTask");
BluetoothTask BTTask("BluetoothTask");
MCUToMCUTask ProcToProc(&CmdTask, "ProcToProc");
DisplayTask ESPDisplayTask("Display");

MCUToMCUTask &getMCUToMCU() {
	return ProcToProc;
}

BluetoothTask &getBLETask() {
	return BTTask;
}

DisplayTask &getDisplayTask() {
	return ESPDisplayTask;
}

#define LED_PIN GPIO_NUM_14

static void IRAM_ATTR gpio_isr_handler(void* arg) {
	uint32_t gpio_num = (uint32_t) arg;
	static int32_t cnt = 0;
	gpio_set_level(LED_PIN,++cnt%2);
	xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg) {
	uint32_t io_num;
	for(;;) {
		if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
			uint32_t level = (uint32_t) gpio_get_level((gpio_num_t)io_num);
			printf("GPIO[%d] intr, val: %d\n", io_num, level);
			//build test message
			System::logSystemInfo();
			char buf[32] = {'\0'};
			sprintf(&buf[0],"FMem: %u", System::getFreeHeapSize());
			getDisplayTask().showMessage(&buf[0],0,16,4000);
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

#define ESP_INTR_FLAG_DEFAULT 0

void initButton() {
	gpio_config_t io_conf;
	//interrupt of rising edge
	io_conf.intr_type = GPIO_INTR_NEGEDGE;
	//bit mask of the pins, use GPIO0
#define GPIO_INPUT_IO_0 (1ULL << GPIO_NUM_0)
	io_conf.pin_bit_mask = GPIO_INPUT_IO_0;
	//set as input mode    
	io_conf.mode = GPIO_MODE_INPUT;
	//enable pull-up mode
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpio_config(&io_conf);
	gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
	//start gpio task
	xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
	gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
	//hook isr handler for specific gpio pin
	gpio_isr_handler_add(GPIO_NUM_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
	//set up pin for output
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
#define GPIO_OUTPUT_LED_MASK (1ULL << GPIO_NUM_14)
	io_conf.pin_bit_mask = GPIO_OUTPUT_LED_MASK;
	io_conf.pull_down_en =GPIO_PULLDOWN_DISABLE; 
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	gpio_config(&io_conf);
}

////
// Basic plan:
//    uart receive task, deserialized via flat buffers, pushes messages to 
//    appropriate cmd queue, cmd queue handles messages and pushed response
//    to out going queue as a flat buffer class, then out going task will
//    serial via flat buffers then send over uart to stm32
void init() {
	FatFS->mount();
	initButton();
}

void app_main()
{
	nvs_flash_init();
	init();
	ESPDisplayTask.init();
	ESPDisplayTask.start();
	ProcToProc.init(TXD_PIN,RXD_PIN,RX_BUF_SIZE);
	ProcToProc.start();
	CmdTask.init();
	CmdTask.start();
	BTTask.init();
	BTTask.start();

	System::logSystemInfo();
	vTaskDelete(NULL);
}
