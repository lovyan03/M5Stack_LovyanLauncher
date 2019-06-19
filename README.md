M5Stack LovyanLauncher
===

M5Stack用 ランチャー by lovyan03  

## Description
  
![image](https://raw.githubusercontent.com/lovyan03/M5Stack_LovyanLauncher/master/image/img_001.jpg)
  
こんなことが出来ます。  
  
* SD-Updater によるアプリケーション起動  
* WiFi 接続設定および接続情報のPreferencesへの保存  
  * AP一覧から選択接続
  * WPSプッシュボタン方式接続  
  * SmartConfigアプリを使用して接続  
  * APモード+HTTPサーバでスマホ等のブラウザでアクセスして設定  
* 各種ツール
  * SystemInformation 各種情報の表示  
  * I2Cスキャナ(接続中のI2C機器のアドレス一覧の表示)  
  * FTPサーバによるSD/SPIFFSへのファイルアクセス  
  * ベンチマークによるSD/SPIFFS性能測定  
  * SPIFFS領域のフォーマット
  * NVS領域のクリア
* バイナリビューアによるSD/SPIFFS/FLASHの内容表示  
* 電源制御
  * バッテリーへの充電の可否設定  
  * バッテリーからの供給の可否設定  
  * 電源オフ時の負荷接続による起動の可否設定  
  * M5GOボトムのLEDの点灯・消灯制御  
  * DeepSleepコマンドによるスリープ(USB接続中でもスリープ可)  
* OTA  
  * Arduino OTA によるPCからapp0領域への書込み  
  * OTA Rollbackによるapp0領域のアプリの起動  
  
実際の動きを以下のツイートの動画で見る事が出来ます。  
[機能紹介](https://twitter.com/lovyan03/status/1096966245562212352)  
[SD-Updater](https://twitter.com/lovyan03/status/1097126013295681537)  
  
※ バッテリー関係の機能は、I2Cスキャナで0x75に反応がある本体でのみ使用可能です。  
USB接続中に画面左上に充電モードを表示します。(残量ではありません)  
CC=定電流充電中  
CV=定電圧充電中  
FC=満充電  
  
充電初期はCC、一定量充電されるとCVに移行します。  
詳細については検索エンジンで「 [充電 CC CV](https://www.google.com/search?q=%E5%85%85%E9%9B%BB+CC+CV&tbm=isch) 」などで調べてみてください。  
[CCからCVへの移行](https://twitter.com/lovyan03/status/1100380792029470720)  
[バッテリ充電オフ](https://twitter.com/lovyan03/status/1100774010017656832)  
※外部電源がない状況下で給電オフ機能を使ってしまった場合は、一度バッテリを取り外すことで復旧できます。  
  
## Usage
 `BtnA click` : フォーカスを親へ移動  Move focus to parent item.  
 `BtnA hold`  : フォーカスを前へ移動  Move focus to the previous item.  
 `BtnB click` : 選択  Select focus item.  
 `BtnC click/hold` : フォーカスを次へ移動  Move focus to the next item.  
  
FACES/JoyStick/PLUSEncoder/CardKB にも対応しています。  
  
## Requirement
動作には以下のライブラリが必要です。  

* https://github.com/tobozo/M5Stack-SD-Updater/  
* https://github.com/bblanchon/ArduinoJson/  
* https://github.com/lovyan03/M5Stack_TreeView/  
* https://github.com/lovyan03/M5Stack_OnScreenKeyboard/  


## Licence

[LGPL v2.1](https://github.com/lovyan03/M5Stack_LovyanLauncher/blob/master/LICENSE)  

## Author

[lovyan03](https://twitter.com/lovyan03)  
