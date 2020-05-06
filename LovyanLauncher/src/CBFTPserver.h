#ifndef _CBFTPSERVER_H_
#define _CBFTPSERVER_H_

#include "MenuCallBack.h"
#include "Header.h"
#include "ESP32FtpServer.h"
#include <SD.h>
#include <SPIFFS.h>
class CBFTPserver : public MenuCallBack
{
public:
  bool setup(){
    M5.Lcd.setTextFont(0);
    bool flgSD = isSD();
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << (flgSD ? 1 : 6));
    }
    M5.Lcd.drawString(menuItem->title, 10, 10, 2);
    _started = false;
    if (flgSD) {
      SD.end();
      SD.begin( TFCARD_CS_PIN, SPI, 40000000);
    } else {
      SPIFFS.begin();
    }
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.mode(WIFI_MODE_STA);
      WiFi.begin();
      M5.Lcd.drawString("WiFi waiting...", 10, 60, 1);
    }
    counter = 0xFF;
    return true;
  }

  bool loop()
  {
    if (_started) {
      getFTPServ()->handleFTP();
    } else {
      if (WiFi.status() == WL_CONNECTED) {
        startFTP();
      }
    }
    if (++counter == 0) Header.draw();
    return true;
  }

  void close()
  {
    if (_started) {
      getFTPServ()->end();
    }
  }

private:
  uint8_t counter = 0;
  bool _started = false;

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
    _started = true;
  }
  virtual bool isSD() { return false; }
  virtual FtpServer* getFTPServ() = 0;
};
class CBFTPserverSD : public CBFTPserver
{
  FtpServer ftpSrv;
  bool isSD() override { return true; }
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
#endif
