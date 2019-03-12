#ifndef _I2CSCANNER_H_
#define _I2CSCANNER_H_

#include <MenuCallBack.h>
#include "Header.h"

class I2CScanner : public MenuCallBack
{
public:
  bool setup() 
  {
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 1);
    }
    M5.Lcd.drawString("I2C Scanner", 10, 10, 2);
    return true;
  }
  bool loop()
  {
    header.draw();
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(0xFFFF, 0);
    byte dummy = 0;
    for (byte adr = 0; adr <= 0x7F; adr++) {
      int x = (adr % 16) * 20;
      int y = 40 + (adr / 16) * 20;
      M5.Lcd.setCursor(4 + x, 6 + y);
      Wire.beginTransmission(adr);
      Wire.write(&dummy, 0);
      uint16_t color = (Wire.endTransmission() == 0) ? 0xFFFF : 0x39E7;
      M5.Lcd.setTextColor(color, 0);
      M5.Lcd.drawRect(x,y,19,19, color);
      M5.Lcd.printf("%02X", adr);
    }
    delay(10);

    return true;
  }
};

#endif
