#ifndef _MENUCALLBACK_H_
#define _MENUCALLBACK_H_

#include <M5Stack.h>
#include <M5TreeView.h>
#include <M5ButtonDrawer.h>

class MenuItem;

// メニュー選択時のコールバック
struct MenuCallBack {
  M5ButtonDrawer btnDrawer;
  M5TreeView* treeView;
  M5TreeView::eCmd cmd;
  MenuItem* menuItem;
  virtual void operator()(MenuItem* mi) {
    menuItem = mi;
    treeView = ((M5TreeView*)(mi->topItem()));
    M5.Lcd.fillScreen(0);
#ifndef ARDUINO_ODROID_ESP32
    btnDrawer.draw(true);
#endif
    btnDrawer.setText("Back","","");
    if (setup()) {
      do {
        cmd = treeView->checkInput();
#ifndef ARDUINO_ODROID_ESP32
        btnDrawer.draw();
#endif
      } while (cmd != M5TreeView::eCmd::BACK && loop());
      close();
    }
    M5.Lcd.fillScreen(MenuItem::backgroundColor);
  }
  virtual bool setup() { return true; };
  virtual bool loop()  { return false; };
  virtual void close() {};
  virtual ~MenuCallBack() {};
};

#endif
