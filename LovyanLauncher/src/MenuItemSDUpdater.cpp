#include "MenuItemSDUpdater.h"

#include <M5StackUpdater.h>   // https://github.com/tobozo/M5Stack-SD-Updater/
#include <SD.h>
#undef min
#include <algorithm>

void MenuItemSDUpdater::onEnter() {
  if (!name.length()) {
    SD.end();
    SD.begin(TFCARD_CS_PIN);
    // search *.bin files from SD root.
    deleteItems();
    File root = SD.open("/");
    File file = root.openNextFile();
    MenuItemSDUpdater* mi;
    while (file) {
      if (!file.isDirectory()) {
        String name = file.name();
        int idx = name.lastIndexOf('.');
        String ext = name.substring(idx + 1);
        if (ext == "bin" && !name.startsWith("/.")) {
          name = name.substring(1, idx);
          mi = new MenuItemSDUpdater(name, name);
          addItem(mi);
        }
      }
      file = root.openNextFile();
      std::sort(Items.begin(), Items.end(), compareIgnoreCase);
    }
    root.close();
  }
  MenuItem::onEnter();
}

void MenuItemSDUpdater::onFocus() {
  if (name.length()) {
    String filename = "/jpg/" + name + ".jpg";
    if (SD.exists(filename.c_str())) {
      M5.Lcd.drawJpgFile(SD, filename.c_str(), 200, 40);
    } else {
      M5.Lcd.setTextColor(0xFFFF);
      M5.Lcd.drawRect(200, 40, 110, 110, 0xFFFF);
      M5.Lcd.drawCentreString("- no image -", 255, 80, 2);
    }
  }
}

void MenuItemSDUpdater::onDefocus() {
  if (name != "") {
    M5.Lcd.fillRect(200, 40, 120, 140, backgroundColor);
  }
}

void MenuItemSDUpdater::onAfterDraw()
{
}
