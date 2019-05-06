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
    p.begin(GlobalParams::preferName);
    String lastBin = p.getString(GlobalParams::preferKeyLastBin, "");
    p.end();
    SD.end();
    SD.begin( TFCARD_CS_PIN, SPI, 40000000);
    // search *.bin files from SD.
    deleteItems();
    std::vector<MenuItem*> filesItems;
    File root = SD.open(path.length() ? path : "/");
    File file = root.openNextFile();
    MenuItemSDUpdater* mi;
    MenuItemSDUpdater* selectmi = NULL;
    String ptmp;
    String fn;
    String ext;
    while (file) {
      ptmp = file.name();
      fn = ptmp.substring(path.length() + 1);
      if (!file.isDirectory()) {
        int idx = fn.lastIndexOf('.');
        ext = fn.substring(idx + 1);
        ext.toLowerCase();
        fn = fn.substring(0, idx);
        if (ext == "bin" && !fn.startsWith(".") && fn != "menu" && file.size() > 100) {
          mi = new MenuItemSDUpdater(fn, ptmp, false, fn);
          filesItems.push_back(mi);
          if (lastBin == fn) selectmi = mi;
        }
      } else {
        String ftmp = fn;
        ftmp.toLowerCase();
        if (ftmp.startsWith("bin") || ftmp.endsWith("bin")) {
          addItem(new MenuItemSDUpdater(fn, ptmp, true, ""));
        }
      }
      file = root.openNextFile();
    }
    root.close();
    if (selectmi) setFocusItem(selectmi);
    std::sort(Items.begin(), Items.end(), compareIgnoreCase);

    if (!filesItems.empty()) {

      std::sort(filesItems.begin(), filesItems.end(), compareIgnoreCase);
      addItems(filesItems);
    }
  }
  MenuItem::onEnter();
}

String MenuItemSDUpdater::getSubFilePath(String subDir, String suffix) {
  MenuItemSDUpdater* pi = static_cast<MenuItemSDUpdater*>(_parentItem);
  if (pi && pi->path.length()) {
    String filename = pi->path + "/" + subDir + "/" + name + suffix;
    if (SD.exists(filename.c_str())) {
      return filename;
    }
  }
  return path.length()
       ? "/" + subDir + "/" + name + suffix
       : "";
}

static uint8_t progressDraw = 0;
static String imageFilePath = "";
void MenuItemSDUpdater::onFocus() {
  progressDraw = (name.length()) ? 0 : 9;
}

void MenuItemSDUpdater::onFocusLoop() {
  if (progressDraw > 9) return;
  switch (progressDraw) {
    case 1:
      imageFilePath = getSubFilePath("jpg", ".jpg");
      progressDraw = (imageFilePath.length()) ? 2 : 9;
      break;
    case 2:
      if (SD.exists(imageFilePath.c_str())) progressDraw += 2;
      else ++progressDraw;
      break;
    case 3:
      M5.Lcd.setTextColor(0xFFFF);
      M5.Lcd.drawRect(200, 40, 110, 110, 0xFFFF);
      M5.Lcd.drawCentreString("- no image -", 255, 80, 2);
      progressDraw = 9;
      break;
    case 7:
      M5.Lcd.drawJpgFile(SD, imageFilePath.c_str(), 200, 40);
      progressDraw = 9;
      break;
    case 9:
      ++progressDraw;
      break;
    default:
      ++progressDraw;
      break;
  }
}

void MenuItemSDUpdater::onDefocus() {
  if (name != "") {
    M5.Lcd.fillRect(200, 40, 120, 140, backgroundColor);
  }
}

