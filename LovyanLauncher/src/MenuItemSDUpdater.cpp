#include "MenuItemSDUpdater.h"
#include "GlobalParams.h"

#include <M5StackUpdater.h>   // https://github.com/tobozo/M5Stack-SD-Updater/
#include <Preferences.h>
#include <SD.h>
#undef min
#include <algorithm>

void MenuItemSDUpdater::onEnter() {
  if (!name.length()) {
    Preferences p;
    p.begin(preferName);
    String lastBin = p.getString(preferKeyLastBin, "");
    p.end();
    SD.end();
    SD.begin(TFCARD_CS_PIN);
    // search *.bin files from SD.
    deleteItems();
    std::vector<MenuItem*> filesItems;
    File root = SD.open(path.length() ? path : "/");
    File file = root.openNextFile();
    MenuItemSDUpdater* mi;
    while (file) {
      String ptmp = file.name();
      String fn = ptmp.substring(path.length() + 1);
      if (!file.isDirectory()) {
        int idx = fn.lastIndexOf('.');
        String ext = fn.substring(idx + 1);
        fn = fn.substring(0, idx);
        if (ext == "bin" && !fn.startsWith("/.")) {
          mi = new MenuItemSDUpdater(fn, ptmp, false, fn);
          filesItems.push_back(mi);
          if (lastBin == fn) setFocusItem(mi);
        }
      } else {
        if (fn.endsWith("bin")) {
          addItem(new MenuItemSDUpdater(fn, ptmp, true, ""));
        }
      }
      file = root.openNextFile();
    }
    std::sort(Items.begin(), Items.end(), compareIgnoreCase);

    if (!filesItems.empty()) {

      std::sort(filesItems.begin(), filesItems.end(), compareIgnoreCase);
      addItems(filesItems);
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

