# bath-guard-duck-device
<img width="700" alt="スクリーンショット 2024-12-01 14 03 18" src="https://github.com/user-attachments/assets/2d4738ad-85ba-4f6d-9172-5e3f83cc8abb">

## 概要

浴槽での幼児の溺水事故を防止するためのシステムである「おふろみまもりだっく」の、ラバーダックに内蔵するLeafony用のプログラム。
本プログラムを書き込んだLeafonyをラバーダックに内蔵し浴槽に浮かべると見守りを開始し、幼児の入水を検知するとWebサーバを経由して端末に通知を行う。

作品についての詳細は[こちら](https://hackmd.io/@mukpan/H1-DIYvC0)

## 状態遷移の条件

本プロジェクトは3つの状態を持ち、状態遷移の発生に応じて、Webアプリの表示の切り替えや、通知の送信を行う。

状態|説明|遷移条件
-|-|-
休止状態|見守りを休止している状態|陸上に設置
見守り状態|見守りを行っている状態|水上に設置
入水検知状態|幼児の入水を検知した状態|幼児の入水による振動を検知

<img width="700" alt="スクリーンショット 2024-12-01 18 55 54" src="https://github.com/user-attachments/assets/2532bb76-6166-4802-9dee-d670f6d290df">

## 環境構築手順

### 1. PlatformIO IDEをVSCodeに導入する

以下のリンクのLeafony公式が公開している手順に従って導入する。
[PlatformIO IDE for VSCodeの設定](https://docs.leafony.com/docs/environment/esp32/platformio/)

### 2. PlatformIO IDEで本プロジェクトを開く

PlatformIO IDEの`Open Project`を選択し本プロジェクトを開く。

<img width="1017" alt="スクリーンショット 2024-12-01 16 12 47" src="https://github.com/user-attachments/assets/5492f3f6-cd8b-44a2-9f0d-cb1a866c3823">

### 3. 本番用のプロジェクトを選択する

`selectMainFile.h`の`#define SELECT_PRODUCT`以外をコメントアウトすることで、本番用のプログラムを使用する。

### 4. Leafonyに書き込む

VSCodeの下部から`Build`と`Upload`を順番に実行する。

## 実行手順

### 1. Leafony_ESP32-APに接続する

書き込み後、`Leafony_ESP32-AP`という名称のWi-Fiに接続する。

<img width="295" alt="upload_6cf4fc70f1167b5f544e5b9ca89198d2" src="https://github.com/user-attachments/assets/23d229e4-95e3-4e18-9e95-fdc08cde8ef9">

### 2. Leafonyの環境設定を行う

[http://192.168.4.1](http://192.168.4.1)に接続すると、以下のような設定画面が開くため、これを全て入力し設定を完了する。

<img width="713" alt="upload_5ac416edf4ab2db908e975faec223e3c" src="https://github.com/user-attachments/assets/291a25cd-2f2d-43a2-9eda-7661bee5e04c">

名称|説明
-|-
Email|「おふろみまもりだっく」のWebアプリで登録したメールアドレス
SSID|Leafonyが接続するWi-FiのSSID
PASSWORD|Leafonyが接続するWi-Fiのpassword
