#ifndef MCU_TO_MCU_H
#define MCU_TO_MCU_H

//#include "esp_system.h"
//#include "esp_log.h"
//#include "driver/uart.h"
//#include "lib/Task.h"
#include <stdint.h>
#include <string>
#include <thread>
#include "task_handler.h"
#include "command_queue.h"
#include "display_handler.h"
#include <messaging/mcu_message.h>
#include <messaging/stm_to_esp_generated.h>
#include <messaging/esp_to_stm_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <future>

class MCUToMCUTask : public TaskHandler<MCUMessage, 4>
{
public:
   using TaskHandler::TaskHandler<MCUMessage, 4>;
   int32_t processMessage(const uint8_t* data, uint32_t size);
   void send(const flatbuffers::FlatBufferBuilder& fbb);

   void handleTask(std::unique_ptr<const MCUMessage> message);

private:
   inline static const char* LOGTAG = "MCUToMCUTask";
   std::future<std::vector<char>> uartReceiveBytes;
};

#endif
