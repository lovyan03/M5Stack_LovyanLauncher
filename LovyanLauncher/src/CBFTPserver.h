#ifndef _CBFTPSERVER_H_
#define _CBFTPSERVER_H_

#include <MenuCallBack.h>
#include "Header.h"
#include "ESP32FtpServer.h"


class CBFTPserver : public MenuCallBack
{
public:
  FtpServer ftpSrv;

  bool setup(){
    M5.Lcd.setTextFont(0);
    me = this;
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 1);
    }
    M5.Lcd.drawString("FTP Server (SDcard)", 10, 10, 2);


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
    ftpSrv.handleFTP();
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
    ftpSrv.begin(user, pass);
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
};
CBFTPserver* CBFTPserver::me;
bool CBFTPserver::closing;
#endif
