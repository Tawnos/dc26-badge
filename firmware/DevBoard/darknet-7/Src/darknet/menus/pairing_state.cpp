#include "pairing_state.h"
#include "../darknet7.h"
#include <crypto/crypto_helper.h>
#include <messaging/stm_to_esp_generated.h>
#include <messaging/esp_to_stm_generated.h>
#include "gui_list_processor.h"
#include "menu_state.h"

#if !defined VIRTUAL_DEVICE
#include <tim.h>

#endif

using cmdc0de::ErrorType;


enum
{
   BOB_WAITING_FOR_FIRST_TRANSMIT,
   BOB_WAITING_FOR_SECOND_TRANSMIT,
};

PairingState::PairingState(DarkNet7* darknet)
: Darknet7BaseState(darknet) {}

ErrorType PairingState::onInit()
{
   InternalState = FETCHING_DATA;

   // set up defaults
   this->timesRunCalledSinceReset = 0;
   this->msgId = 1;
   this->gotBadgeList = false;
   this->securityConfirmed = false;
   this->bleConnected = false;
   this->isAlice = false;
   CurrentRetryCount = 0;
   memset(&AIC, 0, sizeof(AIC));
   memset(&BRTI, 0, sizeof(BRTI));
   memset(&ATBS, 0, sizeof(ATBS));

   flatbuffers::FlatBufferBuilder fbb;
   auto r = darknet7::CreateBLEScanForDevices(fbb, darknet7::BLEDeviceFilter_BADGE);
   this->ESPRequestID = darknet->nextSeq();
   auto e = darknet7::CreateSTMToESPRequest(fbb, ESPRequestID, darknet7::STMToESPAny_BLEScanForDevices, r.Union());
   darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);

   auto mcu = darknet->getMcuToMcu();
   // Register a callback handler
   const MSGEvent<darknet7::BadgesInArea>* si = 0;
   mcu->getBus().addListener(this, si, mcu);

   const MSGEvent<darknet7::BLESecurityConfirm>* si2 = 0;
   mcu->getBus().addListener(this, si2, mcu);

   const MSGEvent<darknet7::BLEConnected>* si3 = 0;
   mcu->getBus().addListener(this, si3, mcu);

   const MSGEvent<darknet7::BLEMessageFromDevice>* dev2 = 0;
   mcu->getBus().addListener(this, dev2, mcu);

   const MSGEvent<darknet7::BLEPairingComplete>* si5 = 0;
   mcu->getBus().addListener(this, si5, mcu);

   darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);
   darknet->getGUI()->drawString(5, 10, (const char*)"Scanning for Badges");


   // Send the STMToESP Message
   mcu->send(fbb);
   return ErrorType();
}

void PairingState::receiveSignal(MCUToMCU*, const MSGEvent<darknet7::BadgesInArea>* mevt)
{
   this->gotBadgeList = true;
   darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);
   const flatbuffers::Vector<flatbuffers::Offset<darknet7::Badge>>* badges;
   if (mevt->RequestID == this->ESPRequestID)
   {
      if (this->InternalState == FETCHING_DATA)
      {
         badges = mevt->InnerMsg->BadgeList();
         unsigned int len = badges->Length();
         for (unsigned int i = 0; i < len; i++)
         {
            const darknet7::Badge* badge = badges->Get(i);
            sprintf(&this->ListBuffer[i][0], "%s", badge->name()->c_str());
            sprintf(&this->AddressBuffer[i][0], "%s", badge->address()->c_str());
            Items[i].text = &this->ListBuffer[i][0];
            Items[i].id = i;
            Items[i].setShouldScroll();
         }
         darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);
         darknet->getGUI()->drawList(&BadgeList);

         InternalState = DISPLAY_DATA;
         this->timesRunCalledSinceReset = 0;
      }
   }
   return;
}

void PairingState::receiveSignal(MCUToMCU*, const MSGEvent<darknet7::BLESecurityConfirm>* mevt)
{
   InternalState = CONNECTING;
   return;
}

void PairingState::receiveSignal(MCUToMCU*, const MSGEvent<darknet7::BLEConnected>* mevt)
{
   darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);
   bool success = mevt->InnerMsg->success();
   if (!success)
      InternalState = PAIRING_FAILED;
   this->isAlice = mevt->InnerMsg->isAlice();
   this->msgId = 1;
   if (this->isAlice)
      InternalState = ALICE_SEND_ONE; // Alice Sends First
   else
      InternalState = RECEIVE_DATA; // Bob Receives first

   this->timesRunCalledSinceReset = 0;
   return;
}

void PairingState::receiveSignal(MCUToMCU*, const MSGEvent<darknet7::BLEMessageFromDevice>* mevt)
{
   const flatbuffers::String* tmesg = mevt->InnerMsg->data();
   memset(&this->MesgBuf, 0, sizeof(MesgBuf));
   this->MesgLen = tmesg->Length();
   memcpy(&this->MesgBuf, tmesg->c_str(), this->MesgLen);
   this->MesgBuf[MesgLen] = 0x0;
   AliceInitConvo* AIC = (AliceInitConvo*)&this->MesgBuf;
   if (AIC->irmsgid == ALICE_SEND_ONE)
      InternalState = BOB_SEND_ONE;
   else if (AIC->irmsgid == ALICE_SEND_TWO)
      InternalState = BOB_SEND_TWO;
   else if (AIC->irmsgid == BOB_SEND_ONE)
      InternalState = ALICE_SEND_TWO;
   else
      InternalState = PAIRING_FAILED;
   this->msgId = 2;
   this->timesRunCalledSinceReset = 0;
   return;
}

void PairingState::receiveSignal(MCUToMCU*, const MSGEvent<darknet7::BLEPairingComplete>* mevt)
{
   InternalState = PAIRING_COMPLETE;
   this->timesRunCalledSinceReset = 0;
   return;
}

void PairingState::CleanUp()
{
   auto mcu = darknet->getMcuToMcu();
   const MSGEvent<darknet7::BadgesInArea>* mevt = 0;
   mcu->getBus().removeListener(this, mevt, mcu);
   const MSGEvent<darknet7::BLESecurityConfirm>* si2 = 0;
   mcu->getBus().removeListener(this, si2, mcu);
   const MSGEvent<darknet7::BLEConnected>* si3 = 0;
   mcu->getBus().removeListener(this, si3, mcu);
   const MSGEvent<darknet7::BLEMessageFromDevice>* si4 = 0;
   mcu->getBus().removeListener(this, si4, mcu);
   const MSGEvent<darknet7::BLEPairingComplete>* si5 = 0;
   mcu->getBus().removeListener(this, si5, mcu);
}

Darknet7BaseState* PairingState::onRun()
{
   Darknet7BaseState* nextState = this;
   auto mcu = darknet->getMcuToMcu();
   flatbuffers::FlatBufferBuilder fbb;
   if (InternalState == FETCHING_DATA)
   {
      if (this->timesRunCalledSinceReset > 500)
      {
         this->CleanUp();
         nextState = darknet->getDisplayMessageState(darknet->getDisplayMenuState(), cmdc0de::NO_DATA_FROM_ESP, 2000);
      }
   }
   else if (InternalState == DISPLAY_DATA)
   {
      if (!GUIListProcessor::process(darknet->getButtonInfo(), &BadgeList, (sizeof(Items) / sizeof(Items[0]))))
      {
         if (darknet->getButtonInfo()->wereAnyOfTheseButtonsReleased(ButtonPress::Fire))
         {
            // send connect message with address
            auto sdata = fbb.CreateString((char*)&this->AddressBuffer[BadgeList.selectedItem][0], 17);
            auto r = darknet7::CreateBLEPairWithDevice(fbb, sdata);
            this->ESPRequestID = darknet->nextSeq();
            auto e = darknet7::CreateSTMToESPRequest(fbb, ESPRequestID, darknet7::STMToESPAny_BLEPairWithDevice, r.Union());
            darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);

            InternalState = INITIATING_CONNECTION;
            mcu->send(fbb); // send the connect message
            this->timesRunCalledSinceReset = 0;
            nextState = this;
         }
         else if (darknet->getButtonInfo()->wereAnyOfTheseButtonsReleased(ButtonPress::Mid))
         {
            this->CleanUp();
            nextState = darknet->getDisplayMenuState();
         }
      }
      darknet->getGUI()->drawList(&BadgeList);
   }
   else if (InternalState == INITIATING_CONNECTION)
   {
      darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);
      darknet->getGUI()->drawString(5, 10, (const char*)"Connecting");
      if (this->timesRunCalledSinceReset > 2000)
         this->InternalState = PAIRING_FAILED;
   }
   else if (InternalState == CONNECTING)
   {
      darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);
      darknet->getGUI()->drawString(5, 10, (const char*)"Connect with This Badge?");
      darknet->getGUI()->drawString(5, 20, (const char*)"Verify Number On Second Screen");
      darknet->getGUI()->drawString(5, 30, (const char*)"Fire1: YES");
      darknet->getGUI()->drawString(5, 40, (const char*)"MID  : NO");
      if (darknet->getButtonInfo()->wereAnyOfTheseButtonsReleased(ButtonPress::Mid))
      {
         auto r = darknet7::CreateBLESendPINConfirmation(fbb, darknet7::RESPONSE_SUCCESS_False);
         auto e = darknet7::CreateSTMToESPRequest(fbb, 0, darknet7::STMToESPAny_BLESendPINConfirmation, r.Union());
         darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
         nextState = darknet->getDisplayMenuState();
         mcu->send(fbb); // send the connect message
      }
      else if (darknet->getButtonInfo()->wereAnyOfTheseButtonsReleased(ButtonPress::Fire))
      {
         darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);;

         InternalState = CONFIRMING;

         //Accept
         auto r = darknet7::CreateBLESendPINConfirmation(fbb, darknet7::RESPONSE_SUCCESS_True);
         auto e = darknet7::CreateSTMToESPRequest(fbb, 0, darknet7::STMToESPAny_BLESendPINConfirmation, r.Union());
         darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
         mcu->send(fbb); // send the connect message
      }
   }
   else if (InternalState == CONFIRMING)
   {
      darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);
      darknet->getGUI()->drawString(5, 10, (const char*)"Confirming");
      if (this->timesRunCalledSinceReset > 1500)
         this->InternalState = PAIRING_FAILED;
   }
   else if (InternalState == ALICE_SEND_ONE)
   {
      darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);
      darknet->getGUI()->drawString(5, 20, (const char*)"Waiting");
      // Wait for 1/2 a second until BOB is probably set up
      HAL_Delay(1000);
      darknet->getGUI()->fillScreen(cmdc0de::RGBColor::BLACK);
      darknet->getGUI()->drawString(5, 20, (const char*)"Alice Send 1");
      //Make the Message
      AIC.irmsgid = ALICE_SEND_ONE;
      memcpy(&AIC.AlicePublicKey[0], darknet->getContactStore()->getMyInfo().getCompressedPublicKey(), PUBLIC_KEY_COMPRESSED_LENGTH);
      AIC.AliceRadioID = darknet->getContactStore()->getMyInfo().getUniqueID();
      strncpy(&AIC.AliceName[0], darknet->getContactStore()->getSettings().getAgentName(), sizeof(AIC.AliceName));

      //Send the message
      auto sdata = fbb.CreateString((char*)&AIC, sizeof(AIC));
      auto r = darknet7::CreateBLESendDataToDevice(fbb, sdata);
      auto e = darknet7::CreateSTMToESPRequest(fbb, 0, darknet7::STMToESPAny_BLESendDataToDevice, r.Union());
      darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
      mcu->send(fbb);

      InternalState = RECEIVE_DATA;
      nextState = this;
   }
   else if (InternalState == ALICE_SEND_TWO)
   {
      darknet->getGUI()->drawString(5, 40, (const char*)"Alice Data 2");

      BobReplyToInit* brti = (BobReplyToInit*)MesgBuf;
      //using signature validate our data that bob signed
      uint8_t uncompressedPublicKey[PUBLIC_KEY_LENGTH];
      uECC_decompress(&brti->BoBPublicKey[0], &uncompressedPublicKey[0], THE_CURVE);
      uint8_t msgHash[SHA256_HASH_SIZE];
      ShaOBJ msgHashCtx;
      sha256_init(&msgHashCtx);
      uint16_t radioID = darknet->getContactStore()->getMyInfo().getUniqueID();
      sha256_add(&msgHashCtx, (uint8_t*)&radioID, sizeof(uint16_t));
      sha256_add(&msgHashCtx, (uint8_t*)darknet->getContactStore()->getMyInfo().getCompressedPublicKey(),
         PUBLIC_KEY_COMPRESSED_LENGTH);
      sha256_digest(&msgHashCtx, &msgHash[0]);

      ATBS.irmsgid = ALICE_SEND_TWO;
      if (uECC_verify(&uncompressedPublicKey[0], &msgHash[0], sizeof(msgHash), &brti->SignatureOfAliceData[0], THE_CURVE))
      {
         uint8_t message_hash[SHA256_HASH_SIZE];
         ShaOBJ messageHashCtx;
         sha256_init(&messageHashCtx);
         sha256_add(&messageHashCtx, (uint8_t*)&brti->BoBRadioID, sizeof(brti->BoBRadioID));
         sha256_add(&messageHashCtx, (uint8_t*)&brti->BoBPublicKey[0], sizeof(brti->BoBPublicKey));
         sha256_digest(&messageHashCtx, &message_hash[0]);
         uint8_t tmp[32 + 32 + 64];
         ATBS.irmsgid = ALICE_SEND_TWO;
         SHA256_HashContext ctx = { { &init_SHA256, &update_SHA256, &finish_SHA256, 64, 32, tmp } };
         uECC_sign_deterministic((const unsigned char*)darknet->getContactStore()->getMyInfo().getPrivateKey(),
            message_hash, sizeof(message_hash), &ctx.uECC, &ATBS.signature[0], THE_CURVE);

         //Add to contacts
         Contact c;
         if (!darknet->getContactStore()->findContactByID(brti->BoBRadioID, c))
         {
            darknet->getContactStore()->addContact(brti->BoBRadioID, &brti->BobAgentName[0],
               &brti->BoBPublicKey[0], &brti->SignatureOfAliceData[0]);
         }
      }
      else
      {
         //	sprintf(&displayBuf[0], "Signature Check Failed with %s", &brti->BobAgentName[0]);
      }

      auto sdata2 = fbb.CreateString((char*)&ATBS, sizeof(ATBS));
      auto r = darknet7::CreateBLESendDataToDevice(fbb, sdata2);
      auto e = darknet7::CreateSTMToESPRequest(fbb, 0, darknet7::STMToESPAny_BLESendDataToDevice, r.Union());
      darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
      mcu->send(fbb);

      InternalState = RECEIVE_DATA;
   }
   else if (InternalState == BOB_SEND_ONE)
   {
      darknet->getGUI()->drawString(5, 20, (const char*)"BOB Send 1");

      AliceInitConvo* aic = (AliceInitConvo*)MesgBuf;
      memcpy(&AIC, aic, sizeof(AIC));
      uint8_t message_hash[SHA256_HASH_SIZE];
      ShaOBJ messageHashCtx;
      sha256_init(&messageHashCtx);
      sha256_add(&messageHashCtx, (uint8_t*)&aic->AliceRadioID, sizeof(aic->AliceRadioID));
      sha256_add(&messageHashCtx, (uint8_t*)&aic->AlicePublicKey, sizeof(aic->AlicePublicKey));
      sha256_digest(&messageHashCtx, &message_hash[0]);
      uint8_t signature[SIGNATURE_LENGTH];

      uint8_t tmp[32 + 32 + 64];
      SHA256_HashContext ctx = { { &init_SHA256, &update_SHA256, &finish_SHA256, 64, 32, &tmp[0] } };
      uECC_sign_deterministic(darknet->getContactStore()->getMyInfo().getPrivateKey(), message_hash,
         sizeof(message_hash), &ctx.uECC, signature, THE_CURVE);
      BRTI.irmsgid = BOB_SEND_ONE;
      BRTI.BoBRadioID = darknet->getContactStore()->getMyInfo().getUniqueID();
      memcpy(&BRTI.BoBPublicKey[0], darknet->getContactStore()->getMyInfo().getCompressedPublicKey(),
         sizeof(BRTI.BoBPublicKey));
      strncpy(&BRTI.BobAgentName[0], darknet->getContactStore()->getSettings().getAgentName(),
         sizeof(BRTI.BobAgentName));
      memcpy(&BRTI.SignatureOfAliceData[0], &signature[0], sizeof(BRTI.SignatureOfAliceData));

      // TODO: Get the data

      auto sdata = fbb.CreateString((char*)&BRTI, sizeof(BRTI));
      auto r = darknet7::CreateBLESendDataToDevice(fbb, sdata);
      auto e = darknet7::CreateSTMToESPRequest(fbb, 0, darknet7::STMToESPAny_BLESendDataToDevice, r.Union());
      darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
      mcu->send(fbb);

      InternalState = RECEIVE_DATA;
   }
   else if (InternalState == BOB_SEND_TWO)
   {
      AliceToBobSignature* atbs = (AliceToBobSignature*)&this->MesgBuf;
      uint8_t uncompressedPublicKey[PUBLIC_KEY_LENGTH];
      uECC_decompress(&AIC.AlicePublicKey[0], &uncompressedPublicKey[0], THE_CURVE);
      uint8_t msgHash[SHA256_HASH_SIZE];
      ShaOBJ msgHashCtx;
      sha256_init(&msgHashCtx);
      uint16_t radioID = darknet->getContactStore()->getMyInfo().getUniqueID();
      sha256_add(&msgHashCtx, (uint8_t*)&radioID, sizeof(uint16_t));
      sha256_add(&msgHashCtx, (uint8_t*)darknet->getContactStore()->getMyInfo().getCompressedPublicKey(), PUBLIC_KEY_COMPRESSED_LENGTH);
      //verify alice's signature of my public key and unique id
      sha256_digest(&msgHashCtx, &msgHash[0]);
      if (uECC_verify(&uncompressedPublicKey[0], &msgHash[0], sizeof(msgHash), &atbs->signature[0], THE_CURVE))
      {
         Contact c;
         if ((darknet->getContactStore()->findContactByID(AIC.AliceRadioID, c) ||
            darknet->getContactStore()->addContact(AIC.AliceRadioID, &AIC.AliceName[0], &AIC.AlicePublicKey[0], &atbs->signature[0])))
         {
            darknet->getGUI()->drawString(5, 40, (const char*)"BOB Send Complete");
            auto r = darknet7::CreateBLESendDNPairComplete(fbb);
            auto e = darknet7::CreateSTMToESPRequest(fbb, 0, darknet7::STMToESPAny_BLESendDNPairComplete, r.Union());
            darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
            mcu->send(fbb);
            InternalState = PAIRING_SUCCESS; // Don't initiate the disconnect as Bob
         }
         else
            InternalState = PAIRING_FAILED;
      }
      else
         InternalState = PAIRING_FAILED;
   }
   else if (InternalState == RECEIVE_DATA)
   {
      if (this->isAlice)
      {
         if (this->msgId == 1)
            darknet->getGUI()->drawString(5, 30, (const char*)"Alice Receive 1");
         else
            darknet->getGUI()->drawString(5, 50, (const char*)"Alice Receive 2");
         if (this->timesRunCalledSinceReset > 500)
            InternalState = PAIRING_FAILED;
      }
      else
      {
         if (this->msgId == 1)
            darknet->getGUI()->drawString(5, 10, (const char*)"BOB Receiving 1");
         else if (this->msgId == 2)
            darknet->getGUI()->drawString(5, 30, (const char*)"BOB Receiving 2");
         if (this->timesRunCalledSinceReset > 500)
            InternalState = PAIRING_FAILED;
      }
   }
   else if (InternalState == PAIRING_COMPLETE)
   {
      auto r = darknet7::CreateBLEDisconnect(fbb);
      this->ESPRequestID = darknet->nextSeq();
      auto e = darknet7::CreateSTMToESPRequest(fbb, ESPRequestID, darknet7::STMToESPAny_BLEDisconnect, r.Union());
      darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
      mcu->send(fbb);
      InternalState = PAIRING_SUCCESS;
   }
   else if (InternalState == PAIRING_SUCCESS)
   {
      // We don't want to disconnect as bob, Alice initiates that when she receives the final message. Report success
      this->CleanUp();
      nextState = darknet->getDisplayMessageState(darknet->getDisplayMenuState(), cmdc0de::BLE_PAIRING_SUCCESS, 2000);
   }
   else if (InternalState == PAIRING_FAILED)
   {
      auto r = darknet7::CreateBLEDisconnect(fbb);
      this->ESPRequestID = darknet->nextSeq();
      auto e = darknet7::CreateSTMToESPRequest(fbb, ESPRequestID, darknet7::STMToESPAny_BLEDisconnect, r.Union());
      darknet7::FinishSizePrefixedSTMToESPRequestBuffer(fbb, e);
      mcu->send(fbb);
      this->CleanUp();
      nextState = darknet->getDisplayMessageState(darknet->getDisplayMenuState(), cmdc0de::BLE_PAIRING_FAILED, 2000);
   }
   this->timesRunCalledSinceReset += 1;
   return nextState;
}

ErrorType PairingState::onShutdown()
{
   return ErrorType();
}
