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
#include <messaging/mcu_message.h>
#include <messaging/stm_to_esp_generated.h>
#include <messaging/esp_to_stm_generated.h>

namespace flatbuffers {
   class FlatBufferBuilder;
}

class CmdHandlerTask;


class MCUToMCUTask : public TaskHandler<MCUMessage, 4>
{
public:
   int32_t processMessage(const uint8_t* data, uint32_t size, std::stop_token stoken);
   void send(const flatbuffers::FlatBufferBuilder& fbb);

   virtual void run(std::stop_token stoken);
protected:
   CmdHandlerTask* CmdHandler;
   std::jthread uartSendThread{ &MCUToMCUTask::uart_send_thread, this };

private:
   inline static const char* LOGTAG = "MCUToMCUTask";
   void uart_send_thread(std::stop_token stoken)
   {
      while (!stoken.stop_requested())
      {
         auto m = getQueue().pop();
         //if (xQueueReceive(OutgoingQueueHandle, &m, portMAX_DELAY))
         if (m)
         {
            uint32_t bytesSent = 0;
            ESP_LOGI(LOGTAG, "sending %d bytes of msg type %d with break!",
               (int)m->getMessageSize(), (int)m->asESPToSTM()->Msg_type());
#if !defined VIRTUAL_DEVICE
            bytesSent = uart_write_bytes_with_break(UART_NUM_1, m->getMessageData(), m->getMessageSize(), 16);
#endif
            if (m->getMessageSize() != bytesSent)
            {
               //ESP_LOGI(MCUToMCUTask::LOGTAG, "failed to send all bytes! %u of %u", bytesSent, m->getMessageSize());
            }
            else
            {
               //ESP_LOGI(MCUToMCUTask::LOGTAG, "Sent!");
            }
         }
      }
   }
};

#endif
