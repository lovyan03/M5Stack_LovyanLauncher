#ifndef _MENUITEMSDUPDATER_H_
#define _MENUITEMSDUPDATER_H_

#include <MenuItemSD.h>

class MenuItemSDUpdater : public MenuItemSD {
public:
  String name;

  MenuItemSDUpdater(const String& titleStr, TCallBackEnter cb = NULL)
  : MenuItemSD(titleStr, cb), name() {};

  MenuItemSDUpdater(const String& t, const String& p, bool isdir, const String& n)
  : MenuItemSD(t, p, isdir), name(n) {};

  String getSubFilePath(String subDir, String suffix);

  virtual void onFocus();
  virtual void onFocusLoop();
  virtual void onDefocus();
  virtual void onEnter();
};

#endif
