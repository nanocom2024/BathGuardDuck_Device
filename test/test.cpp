#include <list>
#include "web_server.h"
// #include "esp32_ap_touch.h"
#include <Adafruit_LIS3DH.h>
#include <BluetoothSerial.h>
#define LIS3DH_ADDRESS 0x19
#define DEVICE_NAME "Leafony_ESP32"
const char WIFI_SSID[] = "Leafony_ESP32-AP";
const char WIFI_PASSWORD[] = "password";
// WiFiServer server(80);
Adafruit_LIS3DH accel = Adafruit_LIS3DH();
BluetoothSerial bts;

const char* ssid = "04F-Pluslab";
const char* password = "bearspooh";
const char* lineToken = ""; // ここにLine Notifyのトークンを入れる
const char* server = "notify-api.line.me";

//加速度の構造体
struct Accel {
    double x;
    double y;
    double z;
};

//直近の加速度のリスト
std::list<Accel> recentAccelList;
//リストの最大長
const int MAX_RECENT_ACCEL = 100;

//プロトタイプ宣言
void sendLineNotify(String message);
void printAccel(Accel accel);

//初期設定
void setup() { 
    Serial.begin(115200);

    if (!bts.begin(DEVICE_NAME)) {
        Serial.println("Bluetooth initialization failed!");
        while (1); // 停止してエラーメッセージを表示する
    } else {
        Serial.println("Bluetooth initialized successfully");
    }

    /* 加速度センサの設定 */
    accel.begin(LIS3DH_ADDRESS);
    accel.setClick(0, 0);                      // Disable Interrupt
    accel.setRange(LIS3DH_RANGE_2_G);          // Full scale +/- 2G
    accel.setDataRate(LIS3DH_DATARATE_10_HZ);  // Data rate = 10Hz
    delay(100);
}



//メインループ
void loop() {
    /* 加速度センサの処理 */
    accel.read();
    //現在の加速度を取得
    Accel nowAccel = {(double)accel.x, (double)accel.y, (double)accel.z};
    // printAccel(nowAccel);
    delay(10);

    //直近の加速度をリストに追加
    recentAccelList.push_back(nowAccel);
    //リストの長さが最大長を超えたら先頭の要素を削除
    if (recentAccelList.size() > MAX_RECENT_ACCEL) {
        recentAccelList.pop_front();
    }
    //加速度リスト中の平均値を計算
    Accel avgAccel = {0, 0, 0};
    for (Accel accel : recentAccelList) {
        avgAccel.x += accel.x;
        avgAccel.y += accel.y;
        avgAccel.z += accel.z;
    }
    avgAccel.x /= recentAccelList.size();
    avgAccel.y /= recentAccelList.size();
    avgAccel.z /= recentAccelList.size();

    //表示
    // printAccel(avgAccel);

    //現在値と平均値の傾き
    Accel slopeAccel = {nowAccel.x / avgAccel.x, nowAccel.y / avgAccel.y, nowAccel.z / avgAccel.z};
    printAccel(slopeAccel);
}


//加速度を表示する関数
void printAccel(Accel accel) {
    bts.print("X: " + String(accel.x));
    bts.print(", ");
    bts.print("Y: " + String(accel.y));
    bts.print(", ");
    bts.print("Z: " + String(accel.z));
    bts.println("");
}
