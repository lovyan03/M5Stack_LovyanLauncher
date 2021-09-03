/*----------------------------------------------------------------------------/

  LovyanLauncher - M5Stack Tools & Application Launcher  

/-----------------------------------------------------------------------------/

Original Source
 https://github.com/lovyan03/M5Stack_LovyanLauncher/  

Licence
 [MIT](https://github.com/lovyan03/M5Stack_LovyanLauncher/blob/master/LICENSE)  

Requirement
 https://github.com/tobozo/M5Stack-SD-Updater/  
 https://github.com/bblanchon/ArduinoJson/  
 https://github.com/lovyan03/M5Stack_TreeView/  
 https://github.com/lovyan03/M5Stack_OnScreenKeyboard/  

Author
 [lovyan03](https://twitter.com/lovyan03)  

/----------------------------------------------------------------------------*/
#include <vector>
#include <M5Stack.h>
#include <M5StackUpdater.h>     // https://github.com/tobozo/M5Stack-SD-Updater/
#include <M5TreeView.h>         // https://github.com/lovyan03/M5Stack_TreeView/
#include <M5OnScreenKeyboard.h> // https://github.com/lovyan03/M5Stack_OnScreenKeyboard/
#include <MenuItemSD.h>
#include <MenuItemSPIFFS.h>
#include <MenuItemToggle.h>
#include <MenuItemWiFiClient.h>
#include <Preferences.h>
#include <esp_sleep.h>
#include <esp_partition.h>
#include <nvs_flash.h>

void cleanup();

#include "src/MenuItemSDUpdater.h"
#include "src/Header.h"
#include "src/SystemInfo.h"
#include "src/I2CScanner.h"
#include "src/WiFiWPS.h"
#include "src/BinaryViewer.h"
#include "src/CBImageViewer.h"
#include "src/CBFTPserver.h"
#include "src/CBArduinoOTA.h"
#include "src/CBSDUpdater.h"
#include "src/CBFSBench.h"
#include "src/CBWiFiSetting.h"
#include "src/CBWiFiSmartConfig.h"
#include "src/GlobalParams.h"

M5TreeView treeView;
M5OnScreenKeyboard osk;
constexpr uint8_t NEOPIXEL_pin = 15;
const char* preferKeyIP5306 PROGMEM( "IP5306CTL0" );
const char* preferKeyStyle  PROGMEM( "TVStyle" );
void drawFrame() {
  Rect16 r = treeView.clientRect;
  r.inflate(1);
  M5.Lcd.drawRect(r.x -1, r.y, r.w +2, r.h, MenuItem::frameColor[1]);
  M5.Lcd.drawRect(r.x, r.y -1, r.w, r.h +2, MenuItem::frameColor[1]);
  M5.Lcd.setTextFont(0);
  M5.Lcd.setTextColor(0x8410,0);
  M5.Lcd.drawString("- LovyanLauncher -", 207, 191, 1);
  M5.Lcd.drawString("@lovyan03    v0.2.3", 204, 201, 1);
  M5.Lcd.drawString("http://git.io/fhdJV", 204, 211, 1);
}

void setStyle(int tag)
{
  switch (tag) {
  default: return;
  case 0:
    M5ButtonDrawer::height = 14;
    M5ButtonDrawer::setTextFont(1);
    treeView.setTextFont(1);
    treeView.itemHeight = 18;
    osk.keyHeight = 14;
    osk.setTextFont(1);
    break;

  case 1:
    M5ButtonDrawer::height = 18;
    M5ButtonDrawer::setTextFont(2);
    treeView.setTextFont(2);
    treeView.itemHeight = 20;
    osk.keyHeight = 18;
    osk.setTextFont(2);
    break;

  case 2:
    M5ButtonDrawer::height = 18;
    M5ButtonDrawer::setTextFont(2);
    treeView.setFreeFont(&FreeSans9pt7b);
    treeView.itemHeight = 24;
    osk.keyHeight = 18;
    osk.setTextFont(2);
    break;
  }
  treeView.updateDest();
  M5.Lcd.fillRect(0, 218, M5.Lcd.width(), 22, 0);
}

void callBackStyle(MenuItem* sender)
{
  setStyle(sender->tag);
  Preferences p;
  p.begin(GlobalParams::preferName);
  p.putUChar(preferKeyStyle, sender->tag);
  p.end();
}

void callBackWiFiClient(MenuItem* sender)
{
  MenuItemWiFiClient* mi = static_cast<MenuItemWiFiClient*>(sender);
  if (!mi) return;

  if (mi->ssid == "") return;

  Preferences preferences;
  preferences.begin("wifi-config");
  preferences.putString("WIFI_SSID", mi->ssid);
  String wifi_passwd = preferences.getString(mi->ssid.c_str());
  if (wifi_passwd == "") wifi_passwd = preferences.getString("WIFI_PASSWD");

  WiFi.disconnect();
  WiFi.mode(WIFI_MODE_STA);
  if (mi->auth != WIFI_AUTH_OPEN) {
    osk.setup(wifi_passwd);
    while (osk.loop()) { delay(1); }
    wifi_passwd = osk.getString();
    osk.close();
    WiFi.begin(mi->ssid.c_str(), wifi_passwd.c_str());
    preferences.putString("WIFI_PASSWD", wifi_passwd);
    preferences.putString(mi->ssid.c_str(), wifi_passwd);
  } else {
    WiFi.begin(mi->ssid.c_str(), "");
    preferences.putString("WIFI_PASSWD", "");
  }
  preferences.end();
  while (M5.BtnA.isPressed()) M5.update();
}

void callBackWiFiOff(MenuItem* sender)
{
  WiFi.mode(WIFI_MODE_STA);
  WiFi.disconnect(true);
}

void callBackFormatSPIFFS(MenuItem* sender)
{
  M5.Lcd.fillRect(20, 100, 160, 30, 0);
  M5.Lcd.drawRect(23, 103, 154, 24, 0xFFFF);
  M5.Lcd.setTextFont(0);
  M5.Lcd.setTextColor(0xFFFF, 0);
  M5.Lcd.drawCentreString("SPIFFS Format...", 90, 106, 2);
  SPIFFS.begin();
  SPIFFS.format();
  SPIFFS.end();
}

void callBackFormatNVS(MenuItem* sender)
{
  M5.Lcd.fillRect(20, 100, 160, 30, 0);
  M5.Lcd.drawRect(23, 103, 154, 24, 0xFFFF);
  M5.Lcd.setTextFont(0);
  M5.Lcd.setTextColor(0xFFFF, 0);
  M5.Lcd.drawCentreString("NVS erase...", 90, 106, 2);
  nvs_flash_init();
  nvs_flash_erase();
  nvs_flash_deinit();
  nvs_flash_init();
  delay(1000);
}

void callBackDeepSleep(MenuItem* sender)
{
  M5.Lcd.setBrightness(0);
  M5.Lcd.sleep();
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_B_PIN, LOW);
  esp_deep_sleep_start();
}

uint8_t getIP5306REG(uint8_t reg, uint8_t defaultValue = 0)
{
  Wire.beginTransmission(0x75);
  Wire.write(reg);
  if (Wire.endTransmission(false) == 0
   && Wire.requestFrom(0x75, 1)) {
    return Wire.read();
  }
  return defaultValue;
}

void setIP5306REG(uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(0x75);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void callBackBatteryIP5306CTL0(MenuItem* sender)
{
  MenuItemToggle* mi((MenuItemToggle*)sender); 
  uint8_t data = getIP5306REG(0, 0x35);
  data = mi->value ? (data | mi->tag) : (data & ~(mi->tag));
  Preferences p;
  p.begin(GlobalParams::preferName);
  p.putUChar(GlobalParams::preferName, data);
  p.end();
  setIP5306REG(0, data);
}

void sendNeoPixelBit(bool flg) {
  for (uint8_t i = 0; i < 2; ++i) digitalWrite(NEOPIXEL_pin, HIGH);
  for (uint8_t i = 0; i < 6; ++i) digitalWrite(NEOPIXEL_pin, flg);
  for (uint8_t i = 0; i < 3; ++i) digitalWrite(NEOPIXEL_pin, LOW);
}
void sendNeoPixelColor(uint32_t color) { // 24bit GRB
  for (uint8_t i = 0; i < 24; ++i) {
    sendNeoPixelBit(color & 0x800000);
    color = color << 1;
  }
}
void setNeoPixelAll(uint32_t color) {
  sendNeoPixelBit(false);
  delay(1);
  for (uint8_t i = 0; i < 10; ++i) {
    sendNeoPixelColor(color);
  }
}
void callBackFIRELED(MenuItem* sender)
{
  MenuItemToggle* mi((MenuItemToggle*)sender); 
  setNeoPixelAll(mi->value ? 0x555555 : 0);
}

void callBackRollBack(MenuItem* sender)
{
  if( Update.canRollBack() )  {
    cleanup();
    Update.rollBack();
    ESP.restart();
  }
}

template <class T>
void callBackExec(MenuItem* sender)
{
  T menucallback;
  menucallback(sender);
}

bool comparePartition(const esp_partition_t* src1, const esp_partition_t* src2, size_t length)
{
    size_t lengthLeft = length;
    const size_t bufSize = SPI_FLASH_SEC_SIZE;
    std::unique_ptr<uint8_t[]> buf1(new uint8_t[bufSize]);
    std::unique_ptr<uint8_t[]> buf2(new uint8_t[bufSize]);
    uint32_t offset = 0;
    size_t i;
    while( lengthLeft > 0) {
      size_t readBytes = (lengthLeft < bufSize) ? lengthLeft : bufSize;
      if (!ESP.flashRead(src1->address + offset, reinterpret_cast<uint32_t*>(buf1.get()), (readBytes + 3) & ~3)
       || !ESP.flashRead(src2->address + offset, reinterpret_cast<uint32_t*>(buf2.get()), (readBytes + 3) & ~3)) {
          return false;
      }
      for (i = 0; i < readBytes; ++i) if (buf1[i] != buf2[i]) return false;
      lengthLeft -= readBytes;
      offset += readBytes;
    }
    return true;
}

bool copyPartition(File* fs, const esp_partition_t* dst, const esp_partition_t* src, size_t length)
{
    M5.Lcd.fillRect( 110, 112, 100, 20, 0);
    size_t lengthLeft = length;
    const size_t bufSize = SPI_FLASH_SEC_SIZE;
    std::unique_ptr<uint8_t[]> buf(new uint8_t[bufSize]);
    uint32_t offset = 0;
    uint32_t progress = 0, progressOld = 0;
    while( lengthLeft > 0) {
      size_t readBytes = (lengthLeft < bufSize) ? lengthLeft : bufSize;
      if (!ESP.flashRead(src->address + offset, reinterpret_cast<uint32_t*>(buf.get()), (readBytes + 3) & ~3)
       || !ESP.flashEraseSector((dst->address + offset) / bufSize)
       || !ESP.flashWrite(dst->address + offset, reinterpret_cast<uint32_t*>(buf.get()), (readBytes + 3) & ~3)) {
          return false;
      }
      if (fs) fs->write(buf.get(), (readBytes + 3) & ~3);
      lengthLeft -= readBytes;
      offset += readBytes;
      progress = 100 * offset / length;
      if (progressOld != progress) {
        progressOld = progress;
        M5.Lcd.progressBar( 110, 112, 100, 20, progress);
      }
    }
    return true;
}

void cleanup() {
  M5.Speaker.end();
}

//======================================================================//
typedef std::vector<MenuItem*> vmi;

void setup() {
  M5.begin();
#ifdef ARDUINO_ODROID_ESP32
  M5.battery.begin();
#else
  M5.Speaker.begin();
  M5.Speaker.mute();
  Wire.begin();
#endif
// for M5GO Bottom LED off
  pinMode(NEOPIXEL_pin, OUTPUT);
  setNeoPixelAll(0);

  const esp_partition_t *running = esp_ota_get_running_partition();
  const esp_partition_t *nextupdate = esp_ota_get_next_update_partition(NULL);
  const char* menubinfilename PROGMEM {MENU_BIN} ;
  if (!nextupdate)
  {
    M5.Lcd.setTextFont(4);
    M5.Lcd.print("! WARNING !\r\nNo OTA partition.\r\nCan't use SD-Updater.");
    delay(3000);
  }
  else if (running && running->label[3] == '0' && nextupdate->label[3] == '1') 
  {
    M5.Lcd.setTextFont(2);
    M5.Lcd.println("LovyanLauncher on app0");
    size_t sksize = ESP.getSketchSize();
    if (!comparePartition(running, nextupdate, sksize))
    {
      bool flgSD = SD.begin( TFCARD_CS_PIN, SPI, 40000000);
      M5.Lcd.print(" copy to app1");
      File dst;
      if (flgSD) {
        dst = (SD.open(menubinfilename, FILE_WRITE ));
        M5.Lcd.print(" and SD menu.bin");
      }
      if (copyPartition( flgSD ? &dst : NULL, nextupdate, running, sksize)) {
        M5.Lcd.println("\r\nDone.");
      }
      if (flgSD) dst.close();
    }
    SDUpdater::updateNVS();      
    M5.Lcd.println("Rebooting app1...");
    if (Update.canRollBack()) {
      Update.rollBack();
      ESP.restart();
    }
  }
  M5.Lcd.fillScreen(0);

  M5ButtonDrawer::width = 106;

  treeView.clientRect.x = 2;
  treeView.clientRect.y = 16;
  treeView.clientRect.w = 196;
  treeView.clientRect.h = 200;
  treeView.itemWidth = 176;

  treeView.useFACES       = true;
  treeView.useCardKB      = true;
  treeView.useJoyStick    = true;
  treeView.usePLUSEncoder = true;
  treeView.useFACESEncoder= true;
  osk.useFACES       = true;
  osk.useCardKB      = true;
  osk.useJoyStick    = true;
  osk.usePLUSEncoder = true;
  osk.useFACESEncoder= true;

  drawFrame();

// restore setting
  Preferences p;
  p.begin(GlobalParams::preferName, true);
  setIP5306REG(0, 0xFD & (p.getUChar(GlobalParams::preferName, getIP5306REG(0, 0x35))
                 |((getIP5306REG(0x70) & 0x04) ? 0x21: 0x01))); //When using battery, Prohibit battery non-use setting.
  setStyle(p.getUChar(preferKeyStyle, 1));
  p.end();

  treeView.setItems(vmi
               { new MenuItemSDUpdater("SD Updater", callBackExec<CBSDUpdater>)
               , new MenuItem("WiFi", vmi
                 { new MenuItemWiFiClient("WiFi Client", callBackWiFiClient)
                 , new MenuItem("WiFi WPS", callBackExec<WiFiWPS>)
                 , new MenuItem("WiFi SmartConfig"     , callBackExec<CBWiFiSmartConfig>)
                 , new MenuItem("WiFi Setting(AP&HTTP)", callBackExec<CBWiFiSetting>)
                 , new MenuItem("WiFi Off", callBackWiFiOff)
                 } )
               , new MenuItem("Tools", vmi
                 { new MenuItem("System Info", callBackExec<SystemInfo>)
                 , new MenuItem("I2C Scanner", callBackExec<I2CScanner>)
                 , new MenuItem("FTP Server (SDCard)", callBackExec<CBFTPserverSD>)
                 , new MenuItem("FTP Server (SPIFFS)", callBackExec<CBFTPserverSPIFFS>)
                 , new MenuItem("Benchmark (SDCard)", callBackExec<CBFSBenchSD>)
                 , new MenuItem("Benchmark (SPIFFS)", callBackExec<CBFSBenchSPIFFS>)
                 , new MenuItem("Format SPIFFS", vmi
                   { new MenuItem("Format Execute", callBackFormatSPIFFS)
                   } )
                 , new MenuItem("Erase NVS(Preferences)", vmi
                   { new MenuItem("Erase Execute", callBackFormatNVS)
                   } )
                 , new MenuItem("Style ", callBackStyle, vmi
                   { new MenuItem("FreeSans9pt7b", 2)
                   , new MenuItem("Font 2" , 1)
                   , new MenuItem("Font 1", 0)
                   } )
                 } )
               , new MenuItem("Binary Viewer", vmi
                 { new MenuItemSD(    "SDCard", callBackExec<BinaryViewerFS>)
                 , new MenuItemSPIFFS("SPIFFS", callBackExec<BinaryViewerFS>)
                 , new MenuItem("FLASH", vmi
                   { new MenuItem("2nd boot loader", 0, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("partition table", 1, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("nvs",         0x102, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("otadata",     0x100, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("app0",        0x010, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("app1",        0x011, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("eeprom",      0x199, callBackExec<BinaryViewerFlash>)
                   , new MenuItem("spiffs",      0x182, callBackExec<BinaryViewerFlash>)
                   } )
                 } )
               , new MenuItem("Image Viewer", vmi
                 { new MenuItemSD(    "SDCard", callBackExec<CBImageViewer>)
                 , new MenuItemSPIFFS("SPIFFS", callBackExec<CBImageViewer>)
                 } )
#ifndef ARDUINO_ODROID_ESP32
               , new MenuItem("Power", vmi
                 { new MenuItemToggle("BatteryCharge" , getIP5306REG(0) & 0x10, 0x10, callBackBatteryIP5306CTL0)
                 , new MenuItemToggle("BatteryOutput" , getIP5306REG(0) & 0x20, 0x20, callBackBatteryIP5306CTL0)
                 , new MenuItemToggle("Boot on load"  , getIP5306REG(0) & 0x04, 0x04, callBackBatteryIP5306CTL0)
                 , new MenuItemToggle("M5GO Bottom LED", false, callBackFIRELED)
                 , new MenuItem("DeepSleep", callBackDeepSleep)
                 })
#endif
               , new MenuItem("OTA", vmi
                 { new MenuItem("Arduino OTA", callBackExec<CBArduinoOTA>)
                 , new MenuItem("OTA Rollback", vmi
                   { new MenuItem("Rollback Execute", callBackRollBack)
                   } )
                 } )
               } );
  treeView.begin();
}

uint8_t loopcnt = 0xF;
long lastctrl = millis();
void loop() {
  if (NULL != treeView.update()) {
    lastctrl = millis();
  }
  if (treeView.isRedraw()) {
    drawFrame();
    loopcnt = 0xF;
  }
  if (0 == (++loopcnt & 0xF)) {
    Header.draw();
#ifndef ARDUINO_ODROID_ESP32
    if ( 600000 < millis() - lastctrl ) {
      Serial.println( "goto sleep" );
      callBackDeepSleep(NULL);
    }
#endif
  }
}
