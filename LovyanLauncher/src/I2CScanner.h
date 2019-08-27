#ifndef _I2CSCANNER_H_
#define _I2CSCANNER_H_

#include <vector>
#include <algorithm>
#include "MenuCallBack.h"
#include "Header.h"
#include "I2CRegistry.h"

class I2CScanner : public MenuCallBack
{
public:
  bool setup() 
  {
    M5.Lcd.fillScreen(0);
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 1);
    }
    M5.Lcd.drawString(menuItem->title, 10, 10, 2);
    btnDrawer.setText("Back/Prev","Ok","Next");
    btnDrawer.draw(true);
    return true;
  }

  bool loop()
  {
    Header.draw();
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(0xFFFF, 0);
    byte dummy = 0;
    uint8_t exist = false;
    std::vector<uint8_t> ex;
    for (uint8_t adr = 0; adr <= 0x7F; adr++) {
      int x = (adr % 16) * 20;
      int y = 40 + (adr / 16) * 20;
      M5.Lcd.setCursor(4 + x, 6 + y);
      Wire.beginTransmission(adr);
      Wire.write(&dummy, 0);
      exist = Wire.endTransmission() == 0;
      if (exist) ex.push_back(adr);
      uint16_t color = exist ? 0xFFFF : 0x39E7;
      M5.Lcd.setTextColor(color, 0);
      if (exist && _addr == adr) {
        color = 0x421F;
      }
      M5.Lcd.drawRect(x,y,19,19, color);
      M5.Lcd.printf("%02X", adr);
    }
    delay(10);

    uint8_t idx = std::distance(ex.begin(), std::lower_bound(ex.begin(), ex.end(), _addr));
    if (cmd != M5TreeView::eCmd::NONE) {
      if (cmd == M5TreeView::eCmd::ENTER && idx < ex.size() && ex[idx] == _addr) {
        I2CRegistry re;
        re.addr = _addr;
        re(menuItem);
        setup();
        return true;
      } else if (cmd == M5TreeView::eCmd::NEXT) {
        idx++;
        if (idx >= ex.size()) idx = 0;
      } else if (cmd == M5TreeView::eCmd::PREV) {
        idx--;
        if (idx >= ex.size()) idx = ex.size() - 1;
      }
    }
    if (idx < ex.size()) _addr = ex[idx];

    return true;
  }

private:
  uint8_t _addr = 0;
};

#endif
