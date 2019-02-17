M5Stack LovyanLauncher
===

M5Stack用 ランチャー by lovyan03  

## Description
  
以下のことが出来ます。  
  
* SD-Updater によるアプリケーション切替  
* WiFi 接続設定および接続情報のPreferencesへの保存  
     (AP一覧から選択接続およびWPS方式を利用した簡単接続)  
* SystemInformation 各種情報の表示  
* I2Cスキャナ(接続中のI2C機器のアドレス一覧の表示)  
* FTPサーバによるSD/SPIFFSへのファイルアクセス  
* バイナリビューアによるSD/SPIFFSのファイル内容表示  
* DeepSleepコマンドによるスリープ(給電中でも可)  


## Usage
 `BtnA click` : フォーカスを親へ移動  Move focus to parent item.  
 `BtnA hold`  : フォーカスを前へ移動  Move focus to the previous item.  
 `BtnB click` : 選択  Select focus item.  
 `BtnC click/hold` : フォーカスを次へ移動  Move focus to the next item.  
  
FACES/JoyStick/PLUSEncoder/CardKB にも対応しています。  
  
## Requirement
動作には以下のライブラリが必要です。  

* https://github.com/tobozo/M5Stack-SD-Updater/  
* https://github.com/lovyan03/M5Stack_TreeView/  
* https://github.com/lovyan03/M5Stack_OnScreenKeyboard/  



## Licence

[LGPL v2.1](https://github.com/lovyan03/M5Stack_LovyanLauncher/blob/master/LICENSE)  

## Author

[lovyan03](https://twitter.com/lovyan03)  
