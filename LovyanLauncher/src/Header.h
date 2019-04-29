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
  String wifiStatus(wl_status_t src);
  int drawBatteryIcon(int x);
};

extern HeaderDrawer Header;

#endif
