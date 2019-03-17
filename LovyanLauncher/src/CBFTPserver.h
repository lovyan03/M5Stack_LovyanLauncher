#ifndef _CBFTPSERVER_H_
#define _CBFTPSERVER_H_

#include <MenuCallBack.h>
#include "Header.h"
#include "ESP32FtpServer.h"
#include <SD.h>
#include <SPIFFS.h>
class CBFTPserver : public MenuCallBack
{
public:
  bool setup(){
    M5.Lcd.setTextFont(0);
    me = this;
    bool flgSD = isSD();
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << (flgSD ? 1 : 6));
    }
    M5.Lcd.drawString(menuItem->title, 10, 10, 2);
    if (flgSD) {
      SD.end();
      SD.begin(TFCARD_CS_PIN);
    } else {
      SPIFFS.begin();
    }
    closing = false;
    onevent = WiFi.onEvent(WiFiEvent);
    if (WiFi.status() == WL_CONNECTED) {
      startFTP();
    } else {
      WiFi.begin();
      M5.Lcd.drawString("WiFi waiting...", 10, 60, 1);
    }

    return true;
  }

  bool loop()
  {
    getFTPServ()->handleFTP();
    if (!(++counter & 0xF)) Header.draw();
    return true;
  }

  void close()
  {
    closing = true;
    WiFi.removeEvent(onevent);
    delay(100);
  }
private:
  long counter = 0;
  wifi_event_id_t onevent = 0;
  static CBFTPserver* me;
  static bool closing;
  static void WiFiEvent(WiFiEvent_t event, system_event_info_t info){
    if (closing) return;
    switch(event){
      case SYSTEM_EVENT_STA_GOT_IP:
        me->startFTP();
        break;
      default:
        break;
    }
  }

  void startFTP()
  {
    String user = "esp32";
    String pass = "esp32";
    getFTPServ()->begin(user, pass);
    M5.Lcd.setTextColor(0xFFFF, 0);
    M5.Lcd.setCursor(0,60);
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("host : " + WiFi.localIP().toString());
    M5.Lcd.println("user : " + user);
    M5.Lcd.println("pass : " + pass);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextFont(2);
    M5.Lcd.print("\r\nftp://" + user + ":" + pass + "@" + WiFi.localIP().toString() + "/");
  }
  virtual bool isSD() { return false; }
  virtual FtpServer* getFTPServ() = 0;
};
class CBFTPserverSD : public CBFTPserver
{
  FtpServer ftpSrv;
  bool isSD() { return true; }
  FtpServer* getFTPServ() {
    return &ftpSrv;
  }
};
class CBFTPserverSPIFFS : public CBFTPserver
{
  FtpServerSPIFFS ftpSrv;
  FtpServer* getFTPServ() {
    return &ftpSrv;
  }
};
CBFTPserver* CBFTPserver::me;
bool CBFTPserver::closing;
#endif
