#ifndef _CBWIFISETTING_H_
#define _CBWIFISETTING_H_

#include <ESPmDNS.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "WebServer.h"
#include "MenuCallBack.h"
#include "Header.h"

class CBWiFiSetting : public MenuCallBack
{
public:
  bool setup() {
    Header.draw();
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF);
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), i << 12);
    }
    M5.Lcd.drawString("WiFi Setting HTTP Server", 10, 10, 2);

    preferences.begin("wifi-config");

    M5.Lcd.setCursor(0,30);
    M5.Lcd.setTextFont(2);
    M5.Lcd.print("Starting Web Server...");

    setupMode();
    return true;
  }

  bool loop() {
    if (!(++counter & 0xF)) Header.draw();
    webServer.handleClient();
    return true;
  }

  void close()
  {
    WiFi.mode(WIFI_MODE_STA);
    preferences.end();
  }

  CBWiFiSetting()
  : MenuCallBack()
  , apIP(192, 168, 4, 1)
  , apSSID("M5STACK_SETUP")
  , webServer(80)
  {}
private:
  long counter = 0;

  const IPAddress apIP;
  const char* apSSID;
  String ssidList;
  String wifi_ssid;
  String wifi_password;

  WebServer webServer;

  // wifi config store
  Preferences preferences;

  void startWebServer() {
    String strAPIP = WiFi.softAPIP().toString();
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print("host : ");
    M5.Lcd.println(strAPIP);
    M5.Lcd.qrcode("http://" + strAPIP, 200, 80, 120, 2);
    webServer.onNotFound([this]() {
      String s = "<h1>Wi-Fi Settings</h1><p>Please enter your password by selecting the SSID.</p>"
                 "<form method=\"get\" action=\"setap\"><label>SSID: </label><select name=\"ssid\">"
                 + ssidList +
                 "</select><br>Password: <input name=\"pass\" length=64 type=\"password\"><input type=\"submit\"></form>";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
    });
    webServer.on("/setap", [this]() {
      M5.Lcd.setTextFont(2);
      M5.Lcd.setTextSize(1);
      String ssid = urlDecode(webServer.arg("ssid"));
      M5.Lcd.print("SSID: ");
      M5.Lcd.println(ssid);
      String pass = urlDecode(webServer.arg("pass"));
      M5.Lcd.print("Password: ");
      M5.Lcd.println(pass);
      M5.Lcd.println("Writing SSID to EEPROM...");

      // Store wifi config
      M5.Lcd.println("Writing Password to nvr...");
      preferences.putString("WIFI_SSID", ssid);
      preferences.putString("WIFI_PASSWD", pass);

      M5.Lcd.println("Write nvr done!");
      String s = "<h1>Setup complete.</h1><p>device will be connected to \""
              + ssid + "\" after the restart.";
      webServer.send(200, "text/html", makePage("Wi-Fi Settings", s));
      delay(3000);
      ESP.restart();
    });
    webServer.begin();
  }

  void setupMode() {
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    delay(100);
    M5.Lcd.println("");
    for (int i = 0; i < n; ++i) {
      ssidList += "<option value=\"";
      ssidList += WiFi.SSID(i);
      ssidList += "\">";
      ssidList += WiFi.SSID(i);
      ssidList += "</option>";
    }
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print("  AP : ");
    M5.Lcd.println(apSSID);
    delay(100);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(apSSID);
    WiFi.mode(WIFI_MODE_AP);
    startWebServer();
  }

  String makePage(String title, String contents) {
    String s = "<!DOCTYPE html><html><head>"
        "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">"
        "<title>"
        + title +
        "</title></head><body>"
        + contents +
        "</body></html>";
    return s;
  }

  String urlDecode(String input) {
    String s = input;
    s.replace("%20", " ");
    s.replace("+", " ");
    s.replace("%21", "!");
    s.replace("%22", "\"");
    s.replace("%23", "#");
    s.replace("%24", "$");
    s.replace("%25", "%");
    s.replace("%26", "&");
    s.replace("%27", "\'");
    s.replace("%28", "(");
    s.replace("%29", ")");
    s.replace("%30", "*");
    s.replace("%31", "+");
    s.replace("%2C", ",");
    s.replace("%2E", ".");
    s.replace("%2F", "/");
    s.replace("%2C", ",");
    s.replace("%3A", ":");
    s.replace("%3A", ";");
    s.replace("%3C", "<");
    s.replace("%3D", "=");
    s.replace("%3E", ">");
    s.replace("%3F", "?");
    s.replace("%40", "@");
    s.replace("%5B", "[");
    s.replace("%5C", "\\");
    s.replace("%5D", "]");
    s.replace("%5E", "^");
    s.replace("%5F", "-");
    s.replace("%60", "`");
    return s;
  }
};
#endif
