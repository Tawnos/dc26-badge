//#include "command_handler.h"
//#include "stm_to_esp_generated.h"
//#include "esp_to_stm_generated.h"
//
//#include "mcu_to_mcu.h"
//#include "npc_interact.h"
//
//#if !defined VIRTUAL_DEVICE
//#include "dc26.h"
//#include "lib/System.h"
//#include "lib/net/HttpServer.h"
//#include <esp_wifi.h>
//#include "dc26_ble/ble.h"
//#include "display_handler.h"
//
//static HttpServer Port80WebServer;
//class MyWiFiEventHandler : public WiFiEventHandler
//{
//public:
//   const char* logTag = "MyWiFiEventHandler";
//   MyWiFiEventHandler() : APStarted(false), scanMsgID(0), WifiFilter(darknet7::WiFiScanFilter_ALL) {}
//public:
//   virtual esp_err_t staGotIp(system_event_sta_got_ip_t event_sta_got_ip)
//   {
//      ESP_LOGD(logTag, "MyWiFiEventHandler(Class): staGotIp");
//      //IP2STR(&event_sta_got_ip.ip_info.ip);
//      return ESP_OK;
//   }
//   virtual esp_err_t apStart()
//   {
//      ESP_LOGI(logTag, "MyWiFiEventHandler(Class): apStart starting web server");
//      display->showMessage("starting AP!", 0, 16, 2000);
//      //Port80WebServer.start(80,false);
//      APStarted = true;
//      return ESP_OK;
//   }
//   virtual esp_err_t apStop()
//   {
//      ESP_LOGI(logTag, "MyWiFiEventHandler(Class): apStop stopping web server");
//      display->showMessage("stopping AP!", 0, 16, 2000);
//      //Port80WebServer.stop();
//      APStarted = false;
//      return ESP_OK;
//   }
//   virtual esp_err_t wifiReady()
//   {
//      ESP_LOGI(logTag, "MyWiFiEventHandler(Class): wifi ready");
//      return ESP_OK;
//   }
//   virtual esp_err_t staConnected(system_event_sta_connected_t info)
//   {
//      display->showMessage("Connected!", 0, 16, 2000);
//      std::string s((const char*)&info.ssid[0], info.ssid_len);
//      ESP_LOGI(logTag, "Connected to: %s on channel %d", s.c_str(), (int)info.channel);
//      return ESP_OK;
//   }
//   virtual esp_err_t staDisconnected(system_event_sta_disconnected_t info)
//   {
//      display->showMessage("Disconnected!", 0, 16, 2000);
//      return ESP_OK;
//   }
//   darknet7::WifiMode convertToWiFiAuthMode(uint16_t authMode)
//   {
//      switch (authMode)
//      {
//      case WIFI_AUTH_OPEN:
//         return darknet7::WifiMode_OPEN;
//      case WIFI_AUTH_WEP:
//         return darknet7::WifiMode_WEP;
//      case WIFI_AUTH_WPA2_PSK:
//         return darknet7::WifiMode_WPA2;
//      case WIFI_AUTH_WPA_PSK:
//         return darknet7::WifiMode_WPA;
//      case WIFI_AUTH_WPA_WPA2_PSK:
//         return darknet7::WifiMode_WPA_WPA2;
//      case WIFI_AUTH_WPA2_ENTERPRISE:
//         return darknet7::WifiMode_WPA2_ENTERPRISE;
//      default:
//         return darknet7::WifiMode_UNKNOWN;
//      }
//   }
//   virtual esp_err_t staScanDone(system_event_sta_scan_done_t info)
//   {
//      ESP_LOGI(logTag, "MyWiFiEventHandler(Class): scan done: APs Found %d", (int32_t)info.number);
//      uint16_t numAPs = info.number;
//      wifi_ap_record_t* recs = new wifi_ap_record_t[numAPs];
//      std::vector<flatbuffers::Offset<darknet7::WiFiScanResult>> APs;
//      flatbuffers::FlatBufferBuilder fbb;
//      int numberToReturn = 0;
//#if !defined VIRTUAL_DEVICE
//      auto result = esp_wifi_scan_get_ap_records(&numAPs, recs);
//#else
//      auto result = ESP_OK;
//#endif
//      if (ESP_OK == result)
//      {
//         for (auto i = 0; i < numAPs; ++i)
//         {
//            if (numberToReturn < 5)
//            {
//               ESP_LOGI(logTag, "ssid: %s \t auth: %d", (const char*)recs[i].ssid, recs[i].authmode);
//               ESP_LOGI(logTag, "%x%x%x", recs[i].bssid[0], recs[i].bssid[1], recs[i].bssid[2]);
//               if (strstr("dark", (const char*)recs[i].ssid) && recs[i].bssid[0] == 0xdc
//                  && recs[i].bssid[1] == 0xd0)
//               {
//                  std::vector<uint8_t> bssid;
//                  for (int kk = 0; kk < 6; kk++) bssid.push_back(recs[i].bssid[kk]);
//                  flatbuffers::Offset<darknet7::WiFiScanResult> sro =
//                     darknet7::CreateWiFiScanResultDirect(fbb,
//                        &bssid, (const char*)recs[i].ssid,
//                        convertToWiFiAuthMode(recs[i].authmode));
//                  APs.push_back(sro);
//                  ++numberToReturn;
//               }
//            }
//         }
//         auto ssro = darknet7::CreateWiFiScanResultsDirect(fbb, &APs);
//         flatbuffers::Offset<darknet7::ESPToSTM> of = darknet7::CreateESPToSTM(fbb,
//            scanMsgID, darknet7::ESPToSTMAny_WiFiScanResults, ssro.Union());
//         darknet7::FinishSizePrefixedESPToSTMBuffer(fbb, of);
//         mcuToMcu->send(fbb);
//      }
//      //esp_wifi_scan_stop();
//      delete[] recs;
//      return ESP_OK;
//   }
//   virtual esp_err_t staAuthChange(system_event_sta_authmode_change_t info)
//   {
//      return ESP_OK;
//   }
//   virtual esp_err_t staStart()
//   {
//      return ESP_OK;
//   }
//   virtual esp_err_t staStop()
//   {
//      return ESP_OK;
//   }
//   bool isAPStarted() { return APStarted; }
//   uint16_t getScanMsgID() { return scanMsgID; }
//   void setScanMsgID(uint16_t sid) { scanMsgID = sid; }
//   void setWiFiScanNPCOnly(bool b)
//   {
//      if (b)
//      {
//         WifiFilter = darknet7::WiFiScanFilter_NPC;
//      }
//      else
//      {
//         WifiFilter = darknet7::WiFiScanFilter_ALL;
//      }
//   }
//private:
//   bool APStarted;
//   uint16_t scanMsgID;
//   darknet7::WiFiScanFilter WifiFilter;
//};
//
//
//wifi_auth_mode_t convertAuthMode(darknet7::WifiMode m)
//{
//   switch (m)
//   {
//   case darknet7::WifiMode_OPEN:
//      return WIFI_AUTH_OPEN;
//      break;
//   case darknet7::WifiMode_WPA2:
//      return WIFI_AUTH_WPA2_PSK;
//      break;
//   case darknet7::WifiMode_WPA:
//      return WIFI_AUTH_WPA_WPA2_PSK;
//      break;
//   default:
//      return WIFI_AUTH_OPEN;
//      break;
//   }
//}
//
//#endif
//
//#define LOGTAG "CmdHandlerTask"
//
//void CmdHandlerTask::run(std::stop_token stoken)
//{
//   ESP_LOGI(LOGTAG, "CommandHandler Task started");
//}
//
