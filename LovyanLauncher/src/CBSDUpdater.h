#ifndef _CBSDUPDATER_H_
#define _CBSDUPDATER_H_

#include <Preferences.h>
#include <ArduinoJson.h>     // https://github.com/bblanchon/ArduinoJson/
#include "MenuCallBack.h"
#include "MenuItemSDUpdater.h"
#include "GlobalParams.h"
#include "Header.h"


class CBSDUpdater : public MenuCallBack
{
public:
  bool setup(){
    mi = (MenuItemSDUpdater*)menuItem;
    if (0 == mi->name.length()) return false;
    btnDrawer.setText(1, "Execute");
    btnDrawer.setText(2, "QRcode");
    btnDrawer.draw(true);
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF);

    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 1);
    }
    M5.Lcd.drawString("SD Updater : " + mi->name, 10, 10, 2);
    iconName = mi->getSubFilePath("jpg", ".jpg");
    faceName = mi->getSubFilePath("jpg", "_gh.jpg");
    metaName = mi->getSubFilePath("json", ".json");
/*
    File file = SD.open( mi->path );
    fileSize = file.size();
    file.close();
*/
    hasIcon = SD.exists(iconName.c_str());
    hasMeta = SD.exists(metaName.c_str());
    hasFace = SD.exists(faceName.c_str());

    drawInfo();
    return true;
  }

  bool loop()
  {
    if (cmd == M5TreeView::eCmd::ENTER) {
      Preferences p;
      p.begin(GlobalParams::preferName);
      p.putString(GlobalParams::preferKeyLastBin, mi->name);
      p.end();
      SDUpdater sdUpdater;
      sdUpdater.updateFromFS(SD, mi->path);
      cleanup();
      ESP.restart();
    }
    if (cmd == M5TreeView::eCmd::NEXT
     || cmd == M5TreeView::eCmd::PREV) {
      isQR = !isQR;
      M5.Lcd.setTextFont(0);
      M5.Lcd.setTextColor(0xFFFF);
      M5.Lcd.fillRect(0,60,M5.Lcd.width(), 160, 0);
      if (isQR) drawQRcode();
      else drawInfo();
    }

    Header.draw();
    delay(100);
    return true;
  }

private:
  struct JSONMeta {
    int width;
    int height;
    String authorName = "";
    String projectURL = "";
    String credits = "";
  };

  MenuItemSDUpdater* mi;

  String metaName;
  String iconName;
  String faceName;
  uint32_t fileSize;
  bool hasIcon = false;
  bool hasMeta = false;
  bool hasFace = false; // github avatar
  JSONMeta jsonMeta;

  bool isQR = false;

  void getMeta( String metaFileName, JSONMeta &jsonMeta ) {
    File file = SD.open( metaFileName );
#if ARDUINOJSON_VERSION_MAJOR==6
    StaticJsonDocument<512> jsonBuffer;
    DeserializationError error = deserializeJson( jsonBuffer, file );
    if (error) return;
    JsonObject root = jsonBuffer.as<JsonObject>();
    if ( !root.isNull() )
#else
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(file);
    if ( root.success() )
#endif
    {
      jsonMeta.width  = root["width"];
      jsonMeta.height = root["height"];
      jsonMeta.authorName = root["authorName"].as<String>();
      jsonMeta.projectURL = root["projectURL"].as<String>();
      jsonMeta.credits    = root["credits"].as<String>();
    }
  }

  void drawInfo()
  {
    M5.Lcd.setTextFont(0);
    if (hasMeta) {
      getMeta(metaName, jsonMeta);
      M5.Lcd.setCursor(0, 30);
      if( jsonMeta.credits != "") {
        M5.Lcd.print( jsonMeta.credits );
      }
      M5.Lcd.setTextFont(2);
      M5.Lcd.setCursor(0, 60);
      if( jsonMeta.authorName != "") {
        M5.Lcd.print( "By " );
        M5.Lcd.print( jsonMeta.authorName );
        M5.Lcd.print( " **" );
      }
    }
    if (hasFace) {
      M5.Lcd.drawJpgFile(SD, faceName.c_str(), 20, 85, 120, 120, 0, 0, JPEG_DIV_NONE );
    } else {
      M5.Lcd.drawRect(20, 80, 120, 120, 0xFFFF);
      M5.Lcd.drawCentreString("- no image -", 80, 140, 2);
    }
    if (hasIcon) {
      M5.Lcd.drawJpgFile(SD, iconName.c_str(), 180, 85, 120, 120, 0, 0, JPEG_DIV_NONE );
    } else {
      M5.Lcd.drawRect(180, 80, 120, 120, 0xFFFF);
      M5.Lcd.drawCentreString("- no image -", 240, 140, 2);
    }
  }

  void drawQRcode()
  {
    if( jsonMeta.projectURL!="" ) { // only projectURL
      M5.Lcd.qrcode(jsonMeta.projectURL, 90, 80, 140, getLowestQRVersionFromString(jsonMeta.projectURL, 2));
      M5.Lcd.setTextFont(2);
      M5.Lcd.setCursor(0, 60);
      M5.Lcd.print( jsonMeta.projectURL );
    } else {
      M5.Lcd.drawRect(90, 80, 140, 140, 0xFFFF);
      M5.Lcd.drawCentreString("- no data -", 160, 140, 2);
    }
  }

  uint8_t getLowestQRVersionFromString( String text, uint8_t ecc ) {
// https://github.com/tobozo/M5Stack-SD-Updater/blob/master/examples/M5Stack-SD-Menu/M5Stack-SD-Menu.ino#L279
    if (ecc>3) return 4; // fail fast
    uint16_t len = text.length();
    uint8_t QRMaxLenByECCLevel[4][3] = {
      // https://www.qrcode.com/en/about/version.html  
      { 25, 47, 77 }, // L
      { 20, 38, 61 }, // M
      { 16, 29, 47 }, // Q
      { 10, 20, 35 }  // H
    };
    for ( uint8_t i=0; i<3; i++ ) {
      if ( len <= QRMaxLenByECCLevel[ecc][i] ) {
        return i+1;
      }
    }
    // there's no point in doing higher with M5Stack's display
    return 4;
  }
};
#endif
