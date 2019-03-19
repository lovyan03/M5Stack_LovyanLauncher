#ifndef _HEADER_H_
#define _HEADER_H_

#include <WiFiType.h>

class HeaderDrawer {
public:
  HeaderDrawer() {};

  uint16_t colorFill = 0x630C;
  uint16_t colorFont = 0xffff;
  uint8_t font = 0;

  void draw();
private:
  int drawStr(const String& src, int x);
  String wifiStatus(wl_status_t src);
  bool readReg(uint8_t* res, uint8_t addr, uint8_t reg);
  const uint8_t IP5306_ADDR = 0x75;
};

extern HeaderDrawer Header;

#endif
