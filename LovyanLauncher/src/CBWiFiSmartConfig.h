#ifndef _CBWIFISMARTCONFIG_H_
#define _CBWIFISMARTCONFIG_H_

#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "MenuCallBack.h"
#include "Header.h"

class CBWiFiSmartConfig : public MenuCallBack
{
public:
  bool setup() {
    Header.draw();
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 6);
    }
    M5.Lcd.drawString("WiFi SmartConfig", 10, 10, 2);

    M5.Lcd.drawCentreString("iOS", 70, 26, 2);
    M5.Lcd.drawCentreString("Android", 250, 26, 2);
    M5.Lcd.qrcode("https://itunes.apple.com/app/id1071176700", 0, 40, 140, 3);                   
    M5.Lcd.qrcode("https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch", 180, 40, 140, 4);

    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();

    M5.Lcd.setCursor(0,180);
    M5.Lcd.setTextFont(2);
    M5.Lcd.print("SmartConfig Started.");
    return true;
  }

  bool loop() {
    if (!(++counter & 0xF)) Header.draw();
    if (WiFi.smartConfigDone() && WiFi.status() == WL_CONNECTED) {
      M5.Lcd.setTextFont(0);
      M5.Lcd.setTextFont(2);
      M5.Lcd.setTextColor(0xFFFF, 0);
      M5.Lcd.println(" Done !");
      M5.Lcd.println("SSID : " + WiFi.SSID());
      Preferences preferences;
      preferences.begin("wifi-config");
      preferences.putString("WIFI_SSID", WiFi.SSID());
      preferences.putString("WIFI_PASSWD", WiFi.psk());
      preferences.end();
      delay(1000);
      return false;
    } else {
      delay(10);
    }
    return true;
  }

  void close()
  {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.stopSmartConfig();
  }

private:
  long counter = 0;
};
#endif
