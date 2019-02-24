#ifndef _HEADERSAMPLE_H_
#define _HEADERSAMPLE_H_

#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiType.h>
#include <esp_wifi.h>

class HeaderSample {
public:
  HeaderSample() {};

  uint16_t colorFill = 0x630C;
  uint16_t colorFont = 0xffff;
  uint8_t font = 0;

  int drawStr(const String& src, int x)
  {
    M5.Lcd.drawString(src, x, 0);
    return x + M5.Lcd.textWidth(src);
  }

  String wifiStatus(wl_status_t src) {
    switch (src)
    {
    case WL_IDLE_STATUS    : return "IDLE_STATUS";
    case WL_NO_SSID_AVAIL  : return "NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED : return "SCAN_COMPLETED";
    case WL_CONNECTED      : return ""; // CONNECTED";
    case WL_CONNECT_FAILED : return "CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "CONNECTION_LOST";
    case WL_DISCONNECTED   : return "DISCONNECTED";
    }
    return "";
  }

  uint8_t chgStatus()
  {
    uint8_t res = 0;
    Wire.beginTransmission(0x75); // IP5306_ADDR
    Wire.write(0x71);             // IP5306_REG_READ1
    Wire.endTransmission(false);
    if (Wire.requestFrom(0x75, 1)) { // IP5306_ADDR
      res = Wire.read();
    }
    return res;
  }
  void draw()
  {
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextFont(font);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(colorFont, colorFill);
    M5.Lcd.drawFastHLine(0, 8, TFT_HEIGHT, 0xC618);

    int x = 0;

    {
      uint8_t bat = chgStatus();
      if (     0x08 == (bat & 0x08)) { x = drawStr("FC ", x); }  // signal of end of charging
      else if (0x80 == (bat & 0x80)) { x = drawStr("?? ", x); }  // (missing description in original document)
      else if (0x60 == (bat & 0x60)) { x = drawStr("CV ", x); }  // Constant Voltage Charging
      else if (0x40 == (bat & 0x40)) { x = drawStr("CC ", x); }  // Constant Current Charging
      else if (0x20 == (bat & 0x20)) { x = drawStr("C ", x); }   // Charging
      else if (0 != bat) { x = drawStr(String(bat, HEX), x); }
    }

    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if (mode == WIFI_AP || mode == WIFI_AP_STA) {
      x = drawStr("AP:", x);
      x = drawStr(WiFi.softAPIP().toString(), x);
      x = drawStr(" ", x);
    }
    if (mode == WIFI_STA || mode == WIFI_AP_STA) {
      wl_status_t s = WiFi.status();
      x = drawStr(wifiStatus(s), x);
      x = drawStr(" ", x);
      if (s == WL_CONNECTED) {
        x = drawStr(WiFi.localIP().toString(), x);
        x = drawStr(" ", x);
      }
    }
    M5.Lcd.setCursor(TFT_HEIGHT - 96, 0);
    M5.Lcd.printf("Free%7d Byte", esp_get_free_heap_size());
    M5.Lcd.fillRect(x, 0, TFT_HEIGHT - 96-x, 8, colorFill);
  }
};

#endif
