#include <vector>
#include <M5Stack.h>
#include <M5StackUpdater.h>     // https://github.com/tobozo/M5Stack-SD-Updater/
#include <M5TreeView.h>         // https://github.com/lovyan03/M5Stack_TreeView
#include <M5OnScreenKeyboard.h> // https://github.com/lovyan03/M5Stack_OnScreenKeyboard/
#include <MenuItemSD.h>
#include <MenuItemSPIFFS.h>
#include <MenuItemWiFiClient.h>
#include <Preferences.h>
#include <esp_sleep.h>

#include "src/MenuItemSDUpdater.h"
#include "src/HeaderSample.h"
#include "src/SystemInfo.h"
#include "src/I2CScanner.h"
#include "src/WiFiWPS.h"
#include "src/BinaryViewer.h"
#include "src/CBFTPserver.h"
#include "src/CBFTPserverSPIFFS.h"

M5TreeView treeView;
M5OnScreenKeyboard osk;
HeaderSample header;

void drawFrame() {
  Rect16 r = treeView.clientRect;
  r.inflate(1);
  M5.Lcd.drawRect(r.x -1, r.y, r.w +2, r.h, MenuItem::frameColor[1]);
  M5.Lcd.drawRect(r.x, r.y -1, r.w, r.h +2, MenuItem::frameColor[1]);
  treeView.update(true);
}

void CallBackWiFiClient(MenuItem* sender)
{
  MenuItemWiFiClient* mi = static_cast<MenuItemWiFiClient*>(sender);
  if (!mi) return;

  if (mi->ssid == "") return;

  Preferences preferences;
  preferences.begin("wifi-config");
  preferences.putString("WIFI_SSID", mi->ssid);
  String wifi_passwd = preferences.getString("WIFI_PASSWD");

  if (mi->auth != WIFI_AUTH_OPEN) {
    osk.setup(wifi_passwd);
    while (osk.loop()) { delay(1); }
    wifi_passwd = osk.getString();
    osk.close();
    WiFi.disconnect();
    WiFi.begin(mi->ssid.c_str(), wifi_passwd.c_str());
    preferences.putString("WIFI_PASSWD", wifi_passwd);
  } else {
    WiFi.disconnect();
    WiFi.begin(mi->ssid.c_str(), "");
    preferences.putString("WIFI_PASSWD", "");
  }
  preferences.end();
  while (M5.BtnA.isPressed()) M5.update();
}

void CallBackStyle(MenuItem* sender)
{
  switch (sender->tag) {
  default: return;
  case 1:
    treeView.font = 1;
    treeView.itemHeight = 18;
    M5ButtonDrawer::height = 14;
    M5ButtonDrawer::width = 64;
    osk.keyHeight = 14;
    break;
  case 2:
    treeView.font = 2;
    treeView.itemHeight = 20;
    M5ButtonDrawer::height = 18;
    M5ButtonDrawer::width = 80;
    osk.keyHeight = 18;
    break;
  }
  M5ButtonDrawer::font = treeView.font;
  osk.font = treeView.font;
  M5.Lcd.fillRect(0, 218, M5.Lcd.width(), 22, 0);
}

void CallBackWiFiDisconnect(MenuItem* sender)
{
  WiFi.disconnect(true);
}

void CallBackDeepSleep(MenuItem* sender)
{
   esp_deep_sleep_start();
}

template <class T>
void CallBackExec(MenuItem* sender)
{
  T menucallback;
  menucallback(sender);
}

//======================================================================//
typedef std::vector<MenuItem*> vmi;

void setup() {
  M5.begin();
  Wire.begin();
  if(digitalRead(BUTTON_A_PIN) == 0) {
     Serial.println("Will Load menu binary");
     updateFromFS(SD);
     ESP.restart();
  }

  Preferences preferences;
  preferences.begin("wifi-config");
  WiFi.begin(preferences.getString("WIFI_SSID").c_str(), preferences.getString("WIFI_PASSWD").c_str());
  preferences.end();

  treeView.clientRect.x = 2;
  treeView.clientRect.y = 16;
  treeView.clientRect.w = 196;
  treeView.clientRect.h = 200;
  treeView.itemWidth = 176;
  treeView.itemHeight = 18;
  treeView.font = 1;

  treeView.useFACES       = true;
  treeView.useCardKB      = true;
  treeView.useJoyStick    = true;
  treeView.usePLUSEncoder = true;
  osk.useFACES       = true;
  osk.useCardKB      = true;
  osk.usePLUSEncoder = true;

  osk.useJoyStick    = true;
  treeView.setItems(vmi
               { new MenuItem("Style ", CallBackStyle, vmi
                 { new MenuItem("Font 2", 2)
                 , new MenuItem("Default", 1)
                 } )
               , new MenuItem("WiFi ", vmi
                 { new MenuItemWiFiClient("WiFi Client", CallBackWiFiClient)
                 , new MenuItem("WiFi WPS", CallBackExec<WiFiWPS>)
                 , new MenuItem("WiFi Disconnect", CallBackWiFiDisconnect)
                 } )
               , new MenuItem("Tools", vmi
                 { new MenuItem("System Info", CallBackExec<SystemInfo>)
                 , new MenuItem("I2C Scanner", I2CScanner())
                 , new MenuItem("FTP Server (SDcard)", CallBackExec<CBFTPserver>)
                 , new MenuItem("FTP Server (SPIFFS)", CallBackExec<CBFTPserverSPIFFS>)
                 } )
               , new MenuItemSDUpdater("SD Updater")
               , new MenuItemSD("SDCard Viewer", CallBackExec<BinaryViewerFS>)
               , new MenuItemSPIFFS("SPIFFS Viewer", CallBackExec<BinaryViewerFS>)
               , new MenuItem("FLASH Viewer", vmi
                 { new MenuItem("2nd boot loader", 0,CallBackExec<BinaryViewerFlash>)
                 , new MenuItem("partation table", 1,CallBackExec<BinaryViewerFlash>)
                 , new MenuItem("nvs",         0x102,CallBackExec<BinaryViewerFlash>)
                 , new MenuItem("otadata",     0x100,CallBackExec<BinaryViewerFlash>)
                 , new MenuItem("app0",        0x010,CallBackExec<BinaryViewerFlash>)
                 , new MenuItem("app1",        0x011,CallBackExec<BinaryViewerFlash>)
                 , new MenuItem("eeprom",      0x199,CallBackExec<BinaryViewerFlash>)
                 , new MenuItem("spiffs",      0x182,CallBackExec<BinaryViewerFlash>)
                 } )
               , new MenuItem("DeepSleep", CallBackDeepSleep)
               } );
  treeView.begin();
  drawFrame();
}

void loop() {
  treeView.update();
  if (treeView.isRedraw()) {
    drawFrame();
  }
  header.draw();
}
