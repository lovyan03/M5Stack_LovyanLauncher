#ifndef _CBFTPSERVER_H_
#define _CBFTPSERVER_H_

#include <MenuCallBack.h>
#include "ESP32FtpServer.h"


class CBFTPserver : public MenuCallBack
{
public:
  FtpServer ftpSrv;

  bool setup(){
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, i, TFT_HEIGHT, i << 1);
    }
    M5.Lcd.drawString("FTP Server (SDcard)", 10, 0, 2);


    ftpSrv.begin("M5","Stack");
    M5.Lcd.setCursor(0,40);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("host : " + WiFi.localIP().toString());
    M5.Lcd.print("user : M5\r\npass : Stack\r\n\r\n");
    M5.Lcd.setTextSize(1);
    M5.Lcd.print("ftp://M5:Stack@" + WiFi.localIP().toString() + "/");
    return true;
  }

  bool loop()
  {
    ftpSrv.handleFTP();

    return true;
  }

private:
};
#endif
