#ifndef _CBFSBENCH_H_
#define _CBFSBENCH_H_

#include <MenuCallBack.h>
#include "Header.h"
#include <SPIFFS.h>
#include <SD.h>

class CBFSBench : public MenuCallBack
{
  String tmpFile{"/lovyanLauncherBench"};
public:
  bool setup() 
  {
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
    M5.Lcd.fillRect(0, 30, M5.Lcd.width(), 160, 0);
    showFSInfo();
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextColor(0xFFFF,0);
    M5.Lcd.setCursor(0, 70);
    M5.Lcd.println("Bench Start");
    uint64_t time = getWriteTime(tmpFile, 1024, 20);
    M5.Lcd.printf("write speed 1KiB:%7d KiB/sec\r\n", (uint64_t)(1000000) * 20 / time);

    time = getReadTime(tmpFile, 1024, 100);
    M5.Lcd.printf("read speed 1KiB:%7d KiB/sec\r\n", (uint64_t)(1000000) * 100 / time);

    time = getWriteTime(tmpFile, 4096, 20);
    M5.Lcd.printf("write speed 4KiB:%7d KiB/sec\r\n", (uint64_t)(4000000) * 20 / time);

    time = getReadTime(tmpFile, 4096, 100);
    M5.Lcd.printf("read speed 4KiB:%7d KiB/sec\r\n", (uint64_t)(4000000) * 100 / time);

    time = getOpenCloseTime(tmpFile, 100);
    M5.Lcd.printf("simple open close:%5d count/sec\r\n", (uint64_t)(1000000) * 100 / time);

    fs::FS& fs = getFS();
    fs.remove(tmpFile);

    M5.Lcd.println("Complete.");
  }

  uint64_t getWriteTime(const String& filepath, long size, int loop) {
    fs::FS& fs = getFS();
    fs.remove(filepath);
    uint8_t *buf = new uint8_t[size];
    for (int i = 0; i < size; ++i) { buf[i] = uint8_t(i & 0xFF); }
    File file = fs.open(filepath, FILE_WRITE);
    uint64_t start = micros();
    for ( int i = 0; i < loop; ++i) {
      file.seek(0);
      file.write(&buf[0], size);
    }
    uint64_t end = micros();
    file.close();
    delete[] buf;
    return end - start;
  }

  uint64_t getReadTime(const String& filepath, long size, int loop) {
    fs::FS& fs = getFS();
    uint8_t *buf = new uint8_t[size];
    File file = fs.open(filepath, FILE_READ);
    uint64_t start = micros();
    for ( int i = 0; i < loop; ++i) {
      file.seek(0);
      file.read(&buf[0], size);
    }
    uint64_t end = micros();
    file.close();
    delete[] buf;
    return end - start;
  }

  uint64_t getOpenCloseTime(const String& filepath, int loop) {
    fs::FS& fs = getFS();
    File f;
    uint64_t start = micros();
    for ( int i = 0; i < loop; ++i) {
      f = fs.open(filepath, FILE_READ);
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
