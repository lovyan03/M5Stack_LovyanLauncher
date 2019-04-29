#include <M5Stack.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "Header.h"

HeaderDrawer Header;

static int drawStr(const String& src, int x)
{
  M5.Lcd.drawString(src, x, 0);
  return x + M5.Lcd.textWidth(src);
}

String HeaderDrawer::wifiStatus(wl_status_t src) {
  switch (src)
  {
  case WL_NO_SHIELD      : return "";
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

static bool readReg(uint8_t* res, uint8_t addr, uint8_t reg)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  if (Wire.endTransmission(false) == 0
   && Wire.requestFrom(addr, (uint8_t)1)) {
    *res = Wire.read();
    return true;
  }
  return false;
}

int HeaderDrawer::drawBatteryIcon(int x)
{
#ifdef ARDUINO_ODROID_ESP32
  int level = M5.battery.getPercentage();
  int w = level / 14;
  M5.Lcd.drawRect(x+1, 1,  9, 5, colorFill);
  M5.Lcd.drawFastHLine(x, 7, 12, colorFill);
  M5.Lcd.drawFastVLine(x + 11, 0, 2, colorFill);
  M5.Lcd.drawFastVLine(x + 11, 5, 2, colorFill);
  if (w < 7) {
    M5.Lcd.fillRect(x+2+w, 2, 7-w, 3, colorFill); 
  }
  uint16_t color = (w == 0) ? 0xF862 : colorFont;
  M5.Lcd.drawFastVLine(x + 11, 2, 3, color);
  M5.Lcd.drawRect(x  , 0, 11, 7, color);
  if (w) {
    M5.Lcd.fillRect(x+2, 2, w, 3, color);
  }
  x += 12;

  M5.Lcd.fillRect(x, 0, 4, 8, colorFill);
  x += 4;
#else
  const uint8_t IP5306_ADDR = 0x75;
  uint8_t r71; 
  if (readReg(&r71, IP5306_ADDR, 0x71)) {
    if (     0x08 == (r71 & 0x08)) { x = drawStr("FC", x); }  // signal of end of charging
    else if (0x80 == (r71 & 0x80)) { x = drawStr("??", x); }  // (missing description in original document)
    else if (0x60 == (r71 & 0x60)) { x = drawStr("CV", x); }  // Constant Voltage Charging
    else if (0x40 == (r71 & 0x40)) { x = drawStr("CC", x); }  // Constant Current Charging
    else if (0x20 == (r71 & 0x20)) { x = drawStr("C ", x); }   // Charging
    else if (0 != r71) { x = drawStr(String(r71, HEX), x); }
    else     {
      uint8_t r78;
      if (readReg(&r78, IP5306_ADDR, 0x78)) {
        r78 &= 0xF0;
        int w = (r78 == 0xE0) ? 1
              : (r78 == 0xC0) ? 3
              : (r78 == 0x80) ? 5
              : (r78 == 0x00) ? 7
              : 0;
        M5.Lcd.drawRect(x+1, 1,  9, 5, colorFill);
        M5.Lcd.drawFastHLine(x, 7, 12, colorFill);
        M5.Lcd.drawFastVLine(x + 11, 0, 2, colorFill);
        M5.Lcd.drawFastVLine(x + 11, 5, 2, colorFill);
        if (w < 7) {
          M5.Lcd.fillRect(x+2+w, 2, 7-w, 3, colorFill); 
        }
        uint16_t color = (r78 == 0xF0) ? 0xF862 : colorFont;
        M5.Lcd.drawFastVLine(x + 11, 2, 3, color);
        M5.Lcd.drawRect(x  , 0, 11, 7, color);
        if (w) {
          M5.Lcd.fillRect(x+2, 2, w, 3, color);
        }
        x += 12;
      }
    }
    M5.Lcd.fillRect(x, 0, 4, 8, colorFill);
    x += 4;
  }
#endif
  return x;
}

void HeaderDrawer::draw()
{
  M5.Lcd.setTextFont(0);
  M5.Lcd.setTextFont(font);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(colorFont, colorFill);
  M5.Lcd.drawFastHLine(0, 8, M5.Lcd.width(), 0xC618);

  int x = drawBatteryIcon(0);

  wifi_mode_t mode;
  if (ESP_OK == esp_wifi_get_mode(&mode)) {
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
  }
  int16_t cx = M5.Lcd.getCursorX();
  int16_t cy = M5.Lcd.getCursorY();
  M5.Lcd.setCursor(M5.Lcd.width() - 96, 0);
  M5.Lcd.printf("Free%7d Byte", esp_get_free_heap_size());
  M5.Lcd.fillRect(x, 0, M5.Lcd.width() - 96-x, 8, colorFill);
  M5.Lcd.setCursor(cx, cy);
}
