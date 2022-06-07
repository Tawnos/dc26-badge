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

namespace darknet7 {
   class STMToESPRequest;
   class ESPToSTM;
}

namespace flatbuffers {
   class FlatBufferBuilder;
}

class CmdHandlerTask;


//We listen for the for our envelop portion of our message which is: 4 bytes:
// bits 0-10 is the size of the message coming max message size = 1024
// bit 11: reserved
// bit 12: reserved
// bit 13: reserved
// bit 14: reserved
// bit 15: reserved
// bit 16-31: CRC 16 of entire message
//set up for our 4 byte envelop header
class MCUToMCUTask : public TaskHandler
{
public:
   static const int ENVELOP_HEADER = 4;
   static const int ENVELOP_HEADER_SIZE_MASK = 0x7FF;
   static const int MAX_MESSAGE_SIZE = 300;
   static const int TOTAL_MESSAGE_SIZE = MAX_MESSAGE_SIZE + ENVELOP_HEADER;
public:
   class Message
   {
   public:
      static const uint16_t MESSAGE_FLAG_TRANSMITTED = 0x8000; //bit 15
   public:
      Message();
      const darknet7::STMToESPRequest* asSTMToESP();
      const darknet7::STMToESPRequest* asSTMToESPVerify();
      const darknet7::ESPToSTM* asESPToSTM();
      bool read(const uint8_t* data, uint32_t dataSize);
      void set(uint16_t sf, uint16_t crc, uint8_t* data);

      void setFlag(uint16_t flags);
      bool checkFlags(uint16_t flags);
      uint16_t getMessageSize() { return getDataSize() + ENVELOP_HEADER; }
      uint16_t getDataSize() { return SizeAndFlags & ENVELOP_HEADER_SIZE_MASK; }
      static uint16_t getDataSize(uint16_t s) { return s & ENVELOP_HEADER_SIZE_MASK; }
      const char* getMessageData() { return (const char*)&MessageData[0]; }
   private:
      uint16_t SizeAndFlags{ 0 };
      uint16_t Crc16{ 0 };
      uint8_t MessageData[MAX_MESSAGE_SIZE]{ 0 };
      friend class MCUToMCUTask;
   };
   //public:
   //	static const int ESP_TO_STM_MSG_QUEUE_SIZE = 4;
   //	static const int ESP_TO_STM_MSG_ITEM_SIZE = sizeof(Message *);
   //	static const char *LOGTAG;
public:
   //MCUToMCUTask(CmdHandlerTask *pch, const std::string &tName, uint16_t stackSize=6000, uint8_t p=5);
   MCUToMCUTask() = default;

   //bool MCUToMCUTask::init(uint8_t tx, uint8_t rx, uint16_t rxBufSize)
   //{
   //	ESP_LOGI(LOGTAG, "INIT");

   //	OutgoingQueueHandle = xQueueCreateStatic(ESP_TO_STM_MSG_QUEUE_SIZE, ESP_TO_STM_MSG_ITEM_SIZE, &ESPToSTMBuffer[0], &OutgoingQueue);
   //	if (OutgoingQueueHandle == nullptr)
   //	{
   //		ESP_LOGI(LOGTAG, "Failed creating OutgoingQueue");
   //	}

   //	uart_config_t uart_config = { .baud_rate = 115200, .data_bits =
   //			UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits =
   //			UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE };
   //	uart_config.use_ref_tick = 0;
   //	uart_param_config(UART_NUM_1, &uart_config);
   //	uart_set_pin(UART_NUM_1, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
   //	// added a buffer for send and receive that we don't have to handle the async send
   //	uart_driver_install(UART_NUM_1, rxBufSize * 2, 0, 0, NULL, 0);
   //	return true;
   //}
   int32_t processMessage(const uint8_t* data, uint32_t size);
   void send(const flatbuffers::FlatBufferBuilder& fbb);

public:
   virtual void run(std::stop_token stoken);
protected:
   CmdHandlerTask* CmdHandler;

private:
   CommandQueue<Message, 4> uartQueue{};
   void uart_send_thread(std::stop_token stoken)
   {
      while (!stoken.stop_requested())
      {
         auto m = uartQueue.pop(stoken);
         //if (xQueueReceive(OutgoingQueueHandle, &m, portMAX_DELAY))
         if (m)
         {
            uint32_t bytesSent = 0;
#if !defined VIRTUAL_DEVICE
            ESP_LOGI(MCUToMCUTask::LOGTAG, "sending %d bytes of msg type %d with break!",
               (int)m->getMessageSize(), (int)m->asESPToSTM()->Msg_type());
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
            delete m;
         }
      }
   }
};

#endif
