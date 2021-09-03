#ifndef _CBARDUINOOTA_H_
#define _CBARDUINOOTA_H_

#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "MenuCallBack.h"
#include "Header.h"
class CBArduinoOTA : public MenuCallBack
{
public:
  bool setup(){
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), (i << 12) | (i << 6));
    }
    M5.Lcd.drawString(menuItem->title, 10, 10, 2);
    _started = false;
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
      ArduinoOTA.handle();
    } else {
      if (WiFi.status() == WL_CONNECTED) {
        startOTA();
      }
    }
    if (++counter == 0) Header.draw();
    return true;
  }

  void close()
  {
    if (_started) {
      ArduinoOTA.end();
    }
  }

private:
  uint8_t counter = 0;
  bool _started = false;

  void startOTA()
  {
    ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        M5.Lcd.println("Start updating " + type);
      })
      .onEnd([]() {
        M5.Lcd.println("\nEnd");
        cleanup();
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        M5.Lcd.progressBar( 110, 112, 100, 20, progress / (total / 100));
      })
      .onError([](ota_error_t error) {
        M5.Lcd.setTextColor(0xFFFF, 0);
        M5.Lcd.setTextFont(1);
        M5.Lcd.printf("Error[%u]: ", error);
        switch (error) {
        case OTA_AUTH_ERROR:    M5.Lcd.println("Auth Failed");   break;
        case OTA_BEGIN_ERROR:   M5.Lcd.println("Begin Failed");  break;
        case OTA_CONNECT_ERROR: M5.Lcd.println("Connect Failed");break;
        case OTA_RECEIVE_ERROR: M5.Lcd.println("Receive Failed");break;
        case OTA_END_ERROR:     M5.Lcd.println("End Failed");    break;
        }
      });

    //ArduinoOTA.setRebootOnSuccess(false);
    ArduinoOTA.begin();

    M5.Lcd.setTextColor(0xFFFF, 0);
    M5.Lcd.setCursor(0,60);
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(2);
    M5.Lcd.println("host : " + WiFi.localIP().toString());
    //M5.Lcd.drawString("host : " + WiFi.localIP().toString(), 0, 60, 1);
    _started = true;
  }
};
#endif
