#ifndef _CBFTPSERVER_H_
#define _CBFTPSERVER_H_

#include <MenuCallBack.h>
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
      M5.Lcd.drawFastHLine(0, i, TFT_HEIGHT, i << 1);
    }
    M5.Lcd.drawString("FTP Server (SDcard)", 10, 0, 2);


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

    return true;
  }

  void close()
  {
    closing = true;
    WiFi.removeEvent(onevent);
    delay(100);
  }

private:
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
    ftpSrv.begin("M5","Stack");
    M5.Lcd.setTextColor(0xFFFF, 0);
    M5.Lcd.setCursor(0,60);
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("host : " + WiFi.localIP().toString());
    M5.Lcd.print("user : M5\r\npass : Stack\r\n\r\n");
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextFont(2);
    M5.Lcd.print("ftp://M5:Stack@" + WiFi.localIP().toString() + "/");
  }
};
CBFTPserver* CBFTPserver::me;
bool CBFTPserver::closing;
#endif
