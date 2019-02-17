#ifndef _MENUITEMSDUPDATER_H_
#define _MENUITEMSDUPDATER_H_

#include <MenuItem.h>

class MenuItemSDUpdater : public MenuItem {
  static uint16_t maxFileCount;
public:
  String name;

  MenuItemSDUpdater(const String& titleStr, std::function<void(MenuItem*)>cb = NULL)
  : MenuItem(titleStr, cb), name() {};

  MenuItemSDUpdater(const String& t, const String& p)
  : MenuItem(t), name(p) {};

  virtual void onFocus();
  virtual void onDefocus();
  virtual void onEnter();
  virtual void onAfterDraw();
};

#endif
