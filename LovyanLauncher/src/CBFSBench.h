#ifndef _CBFSBENCH_H_
#define _CBFSBENCH_H_

#include <SPIFFS.h>
#include <SD.h>
#include "MenuCallBack.h"
#include "Header.h"

class CBFSBench : public MenuCallBack
{
  String tmpFile{"/lovyanLauncherBench"};
public:
  bool setup() 
  {
    Header.draw();
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF);
    fs::FS& fs(getFS());
    for (int i = 1; i < 16; ++i) {
      M5.Lcd.drawFastHLine(0, 10 + i, M5.Lcd.width(), (&fs == &SD) ? (i << 1) : (i << 6));
    }
    M5.Lcd.drawString(menuItem->title, 10, 10, 2);
    showFSInfo();
    return true;
  }

  bool loop()
  {
    if (cmd == M5TreeView::eCmd::ENTER) {
      btnDrawer.setText(1, "");
      btnDrawer.draw();
      ExecBench();
    } else {
      btnDrawer.setText(1, "Execute");
      Header.draw();
      delay(100);
    }
    return true;
  }

protected:
  void showFSInfo() {
    init();
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF, 0);
    M5.Lcd.setCursor(0, 30);

    M5.Lcd.println("total:" + getStrSize(totalBytes()));
    M5.Lcd.println("used :" + getStrSize(usedBytes()));
    M5.Lcd.println("free :" + getStrSize(totalBytes() - usedBytes()));
  }

  String getStrSize(uint64_t value) {
    int base = 0;
    for (; base < 3 && (value >= (100 * 1024)); ++base) {
      value = value >> 10;
    }
    String space = "";
    int dig = (value == 0) ? 0 : log10(value);
    for (int i = 6 - dig; i > 0; --i) space += " ";
    return space + String((long)value, DEC)
         + ((base == 3) ? " GiB"
          : (base == 2) ? " MiB"
          : (base == 1) ? " KiB"
          : (base == 0) ? " Byte"
                        : " ??");
  }

  void ExecBench() {
    M5.Lcd.fillRect(0, 30, M5.Lcd.width(), 180, 0);
    showFSInfo();
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextColor(0xFFFF,0);
    M5.Lcd.setCursor(0, 60);
    M5.Lcd.println("Bench Start");
    uint64_t time;

    int loop = 8;
    time = getWriteTime(tmpFile, 4096, loop);
    M5.Lcd.printf("4KiB Write:%5u KiB/sec", uint32_t((uint64_t)4000000 * loop / time));

    time = getReadTime(tmpFile, 4096, loop);
    M5.Lcd.setCursor(180, M5.Lcd.getCursorY());
    M5.Lcd.printf("Read:%6u KiB/sec\r\n", uint32_t((uint64_t)4000000 * loop / time));

    removeFile(tmpFile, loop);

    loop = 16;
    time = getWriteTime(tmpFile, 1024, loop);
    M5.Lcd.printf("1KiB Write:%5u KiB/sec", uint32_t((uint64_t)1000000 * loop / time));

    time = getReadTime(tmpFile, 1024, loop);
    M5.Lcd.setCursor(180, M5.Lcd.getCursorY());
    M5.Lcd.printf("Read:%6u KiB/sec\r\n", uint32_t((uint64_t)1000000 * loop / time));

    loop = 24;
    time = getWriteTime(tmpFile, 256, loop);
    M5.Lcd.printf("256B Write:%5u KiB/sec", uint32_t((uint64_t)250000 * loop / time));

    time = getReadTime(tmpFile, 256, loop);
    M5.Lcd.setCursor(180, M5.Lcd.getCursorY());
    M5.Lcd.printf("Read:%6u KiB/sec\r\n", uint32_t((uint64_t)250000 * loop / time));

    loop = 32;
    time = getWriteTime(tmpFile, 64, loop);
    M5.Lcd.printf(" 64B Write:%5u KiB/sec", uint32_t((uint64_t)62500 * loop / time));

    time = getReadTime(tmpFile, 64, loop);
    M5.Lcd.setCursor(180, M5.Lcd.getCursorY());
    M5.Lcd.printf("Read:%6u KiB/sec\r\n", uint32_t((uint64_t)62500 * loop / time));

    loop = 40;
    time = getWriteTime(tmpFile, 16, loop);
    M5.Lcd.printf(" 16B Write:%5u KiB/sec", uint32_t((uint64_t)15625 * loop / time));

    time = getReadTime(tmpFile, 16, loop);
    M5.Lcd.setCursor(180, M5.Lcd.getCursorY());
    M5.Lcd.printf("Read:%6u KiB/sec\r\n", uint32_t((uint64_t)15625 * loop / time));

    time = getOpenCloseTime(tmpFile, loop);
    M5.Lcd.printf("simple open close:%4u count/sec\r\n", uint32_t((uint64_t)1000000 * loop / time));

    time = removeFile(tmpFile, loop);
    M5.Lcd.printf("simple file delete:%5u count/sec\r\n", uint32_t((uint64_t)1000000 * loop / time));

    M5.Lcd.println("Complete.");
  }

  uint64_t removeFile(const String& filepath, int loop)
  {
    fs::FS& fs = getFS();
    uint64_t start = micros();
    for (int i = 0; i < loop; ++i) {
      fs.remove(filepath + String(i));
    }
    return micros() - start;
  }

  volatile uint64_t getWriteTime(const String& filepath, long size, int loop) {
    fs::FS& fs = getFS();
    uint8_t buf[4096];
    uint64_t res = 0;
    File f;
    for (int i = 0; i < size; ++i) { buf[i] = uint8_t(i & 0xFF); }
    for ( int i = 0; i < loop; ++i) {
      f = fs.open(filepath + String(i), FILE_WRITE);
      uint64_t start = micros();
      f.write(&buf[0], size);
      f.flush();
      res += micros() - start;
      f.close();
    }
    return res;
  }

  volatile uint64_t getReadTime(const String& filepath, long size, int loop) {
    fs::FS& fs = getFS();
    uint8_t buf[4096];
    uint64_t res = 0;
    File f;
    for ( int i = 0; i < loop; ++i) {
      f = fs.open(filepath + String(i), FILE_READ);
      uint64_t start = micros();
      f.read(&buf[0], size);
      res += micros() - start;
      f.close();
    }
    return res;
  }

  volatile uint64_t getOpenCloseTime(const String& filepath, int loop) {
    fs::FS& fs = getFS();
    File f;
    uint64_t start = micros();
    for ( int i = 0; i < loop; ++i) {
      f = fs.open(filepath + String(i), FILE_READ);
      f.close();
    }
    return micros() - start;
  }
private:
  virtual fs::FS& getFS() = 0;
  virtual void init() = 0;
  virtual uint64_t totalBytes() { return 0; }
  virtual uint64_t usedBytes()  { return 0; }
};

class CBFSBenchSD : public CBFSBench
{
  fs::FS& getFS() { return SD; }
  uint64_t totalBytes() { return SD.totalBytes(); }
  uint64_t usedBytes()  { return SD.usedBytes(); }
  void init()
  {
    SD.end();
    SD.begin(TFCARD_CS_PIN);
  }
};

class CBFSBenchSPIFFS : public CBFSBench
{
public:
  bool setup() 
  {
    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextColor(0xFFFF);
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.print("Format Check...\r\nWait a minute...");
    return CBFSBench::setup();
  }

private:
  fs::FS& getFS() { return SPIFFS; }
  uint64_t totalBytes() { return SPIFFS.totalBytes(); }
  uint64_t usedBytes()  { return SPIFFS.usedBytes(); }
  void init()
  {
    SPIFFS.begin(true);
  }
};
#endif
