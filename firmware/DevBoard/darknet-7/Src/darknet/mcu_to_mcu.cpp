/*
 * mcu_to_mcu.cpp
 *
 *  Created on: Jun 22, 2018
 *      Author: cmdc0de
 */

#if !defined VIRTUAL_DEVICE
#include <usart.h>
#include <logger.h>
#endif

#include "mcu_to_mcu.h"
#include <etl/crc16.h>
#include "../../../GUI/include/virtualHAL.h"

#define ERRMSG(x) // x;


#if !defined VIRTUAL_DEVICE

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
   if (huart == darknet->getMcuToMcu().getUART())
   {
      darknet->getMcuToMcu().handleMcuToMcu();
   }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
   if (huart == darknet->getMcuToMcu().getUART())
   {
      darknet->getMcuToMcu().onError();
   }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
   if (huart == darknet->getMcuToMcu().getUART())
   {
      darknet->getMcuToMcu().onTransmissionComplete();
   }
}

void MCUToMCU::resetUART()
{
   HAL_UART_DeInit(UartHandler);
   init(UartHandler);
}
#endif
MCUToMCU::MCUToMCU(UART_HandleTypeDef* uart) 
   : UartHandler(uart)
{
#if !defined VIRTUAL_DEVICE
   MX_USART1_UART_Init();
   HAL_LIN_Init(UartHandler, UART_LINBREAKDETECTLENGTH_10B);
   HAL_UART_Receive_IT(UartHandler, &UartRXBuffer[0], MCUToMCU::TOTAL_MESSAGE_SIZE);
#endif
}

const darknet7::ESPToSTM* MCUMessage::asESPToSTM()
{
   return darknet7::GetSizePrefixedESPToSTM(&MessageData[ENVELOP_HEADER]);
}

bool MCUMessage::verifyESPToSTM()
{
   flatbuffers::Verifier v(&MessageData[ENVELOP_HEADER], getDataSize());
   return darknet7::VerifySizePrefixedESPToSTMBuffer(v);
}

void MCUToMCU::onError()
{
   handleMcuToMcu();
}

void MCUToMCU::onTransmissionComplete()
{
   MCUMessage& m = OutgoingMessages.front();
   if (m.checkFlags(MCUMessage::MESSAGE_FLAG_TRANSMITTED))
   {
      OutgoingMessages.pop();
      // HAL_LIN_SendBreak(UartHandler);
   }
   //transmitNow();
}

void MCUToMCU::handleMcuToMcu()
{
   uint16_t size = UartHandler->RxXferSize - UartHandler->RxXferCount;
   // we have received something and we have also gotten a line break
   if (size > MCUMessage::ENVELOP_HEADER)// && __HAL_UART_GET_FLAG(UartHandler, UART_FLAG_LBD))
   {
      uint16_t firstTwo = (*((uint16_t*)&UartRXBuffer[0]));
      uint16_t dataSize = firstTwo & 0x7FF; // 0-10 bits are size
      uint16_t crcFromESP = (*((uint16_t*)&UartRXBuffer[2]));
      if (dataSize <= size)
      {
         etl::crc16 crc(&UartRXBuffer[MCUMessage::ENVELOP_HEADER], &UartRXBuffer[MCUMessage::ENVELOP_HEADER] + dataSize);
         if (crc.value() != crcFromESP)
         {
            ERRMSG("CRC ERROR in handle MCU To MCU.\n");
         }
         auto m = MCUMessage{ firstTwo, crcFromESP, &UartRXBuffer[MCUMessage::ENVELOP_HEADER] };
         if (m.verifyESPToSTM())
         {
            IncomingMessages.push(m);
         }
         else
         {
            ERRMSG("invalid message");
         }
#if !defined VIRTUAL_DEVICE
         HAL_UART_AbortReceive_IT(UartHandler);
         HAL_UART_Receive_IT(UartHandler, &UartRXBuffer[0], MAX_MESSAGE_SIZE);
#endif
      }
      else
      {
#if !defined VIRTUAL_DEVICE
         HAL_UART_Receive_IT(UartHandler, &UartRXBuffer[size], MAX_MESSAGE_SIZE - size);
#endif
      }
   }
   else
   {
#if !defined VIRTUAL_DEVICE
      HAL_UART_Receive_IT(UartHandler, &UartRXBuffer[size], MAX_MESSAGE_SIZE - size);
#endif
   }
}

bool MCUToMCU::send(const flatbuffers::FlatBufferBuilder& fbb)
{
   uint8_t* msg = fbb.GetBufferPointer();
   uint32_t size = fbb.GetSize();
   assert(size < MCUMessage::MAX_MESSAGE_SIZE);
   etl::crc16 crc(msg, msg + size);
   OutgoingMessages.push(MCUMessage{ static_cast<uint16_t>(size), crc.value(), msg });

#if !defined VIRTUAL_DEVICE
   if ((UartHandler->gState == HAL_UART_STATE_ERROR || UartHandler->gState == HAL_UART_STATE_TIMEOUT))
   {
      resetUART();
   }
   if ((UartHandler->gState == HAL_UART_STATE_READY) && !OutgoingMessages.empty())
   {
      MCUMessage& m = OutgoingMessages.front();
      m.transmit(UartHandler);
   }
#endif
   return true;
}

void MCUToMCU::run(std::stop_token stoken)
{
   while (!stoken.stop_requested())
   {
      if (!OutgoingMessages.empty()) 
      {
         MCUMessage& m = OutgoingMessages.front();
         if (m.checkFlags(MCUMessage::MESSAGE_FLAG_TRANSMITTED))
         {
            OutgoingMessages.pop();
            // HAL_LIN_SendBreak(UartHandler);
         }
      }
      if (!IncomingMessages.empty())
      {
         MCUMessage& m = IncomingMessages.front();
         const darknet7::ESPToSTM* msg = m.asESPToSTM();
         switch (msg->Msg_type())
         {
         case darknet7::ESPToSTMAny_ESPSystemInfo:
         {
            MSGEvent<darknet7::ESPSystemInfo> mevt(msg->Msg_as_ESPSystemInfo(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_CommunicationStatusResponse:
         {
            MSGEvent<darknet7::CommunicationStatusResponse> mevt(msg->Msg_as_CommunicationStatusResponse(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_WiFiScanResults:
         {
            MSGEvent<darknet7::WiFiScanResults> mevt(msg->Msg_as_WiFiScanResults(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_NPCList:
         {
            MSGEvent<darknet7::NPCList> mevt(msg->Msg_as_NPCList(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_NPCInteractionResponse:
         {
            MSGEvent<darknet7::NPCInteractionResponse> mevt(msg->Msg_as_NPCInteractionResponse(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_BadgesInArea:
         {
            MSGEvent<darknet7::BadgesInArea> mevt(msg->Msg_as_BadgesInArea(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_BLESecurityConfirm:
         {
            MSGEvent<darknet7::BLESecurityConfirm> mevt(msg->Msg_as_BLESecurityConfirm(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_BLEConnected:
         {
            MSGEvent<darknet7::BLEConnected> mevt(msg->Msg_as_BLEConnected(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_BLEMessageFromDevice:
         {
            MSGEvent<darknet7::BLEMessageFromDevice> mevt(msg->Msg_as_BLEMessageFromDevice(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_BLEPairingComplete:
         {
            MSGEvent<darknet7::BLEPairingComplete> mevt(msg->Msg_as_BLEPairingComplete(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         case darknet7::ESPToSTMAny_BLEInfectionData:
         {
            MSGEvent<darknet7::BLEInfectionData> mevt(msg->Msg_as_BLEInfectionData(), msg->msgInstanceID());
            this->getBus().emitSignal(this, mevt);
            break;
         }
         default:
            break;
         }
         IncomingMessages.pop();
      }
   }
}
