#ifndef DARKNET7_MCU_TO_MCU_H
#define DARKNET7_MCU_TO_MCU_H

#include <messaging/mcu_message.h>
#include <messaging/stm_to_esp_generated.h>
#include <messaging/esp_to_stm_generated.h>

#include <etl/vector.h>
#include <etl/queue.h>
#include <observer/event_bus.h>
#include "../../../GUI/include/virtualHAL.h"
#include <thread>

template<typename T>
struct MSGEvent
{
   const T* InnerMsg;
   uint32_t RequestID;
   MSGEvent(const T* im, uint32_t rid) : InnerMsg(im), RequestID(rid) {}
};

class MCUToMCU
{
public:
   using UartEventBus_t = cmdc0de::EventBus<3, 11, 5, 3>;

   void init(UART_HandleTypeDef*);
   bool send(const flatbuffers::FlatBufferBuilder& fbb);
   void process(std::stop_token stoken);
   int32_t processMessage(const uint8_t* data, uint32_t size, std::stop_token stoken);
   void onTransmissionComplete();
   void onError();
   void handleMcuToMcu();
#if !defined VIRTUAL_DEVICE
   const UART_HandleTypeDef* getUART() const { return UartHandler; }
#endif
   UartEventBus_t& getBus()
   {
      return MessageBus;
   }

protected:
#if !defined VIRTUAL_DEVICE
   void resetUART();
#endif

private:
   uint8_t UartRXBuffer[MCUMessage::TOTAL_MESSAGE_SIZE * 2] = { 0 };
   etl::queue<MCUMessage, 4> IncomingMessages{};
   etl::queue<MCUMessage, 4> OutgoingMessages{};
   UART_HandleTypeDef* UartHandler{ 0 };
   UartEventBus_t MessageBus;
};

#endif
