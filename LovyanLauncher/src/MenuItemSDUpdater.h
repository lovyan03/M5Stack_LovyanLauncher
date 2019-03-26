#ifndef _MENUITEMSDUPDATER_H_
#define _MENUITEMSDUPDATER_H_

#include <MenuItemSD.h>

class MenuItemSDUpdater : public MenuItemSD {
  static uint16_t maxFileCount;
public:
  String name;

  MenuItemSDUpdater(const String& titleStr, TCallBackEnter cb = NULL)
  : MenuItemSD(titleStr, cb), name() {};

  MenuItemSDUpdater(const String& t, const String& p, bool isdir, const String& n)
  : MenuItemSD(t, p, isdir), name(n) {};

  virtual void onFocus();
  virtual void onDefocus();
  virtual void onEnter();
};

#endif
