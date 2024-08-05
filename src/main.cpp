// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <WiFiAP.h>
// #include <WiFiClientSecure.h>
// #include <list>
// #include "web_server.h"
// // #include "esp32_ap_touch.h"
// #include <Adafruit_LIS3DH.h>
// #define LIS3DH_ADDRESS 0x19
// const char WIFI_SSID[] = "Leafony_ESP32-AP";
// const char WIFI_PASSWORD[] = "password";
// // WiFiServer server(80);
// Adafruit_LIS3DH accel = Adafruit_LIS3DH();

// const char* ssid = "04F-Pluslab";
// const char* password = "bearspooh";
// const char* lineToken = ""; // ここにLine Notifyのトークンを入れる
// const char* server = "notify-api.line.me";

// //加速度の構造体
// struct Accel {
//     double x;
//     double y;
//     double z;
// };

// //直近の加速度のリスト
// std::list<Accel> recentAccelList;
// //リストの最大長
// const int MAX_RECENT_ACCEL = 100;

// //プロトタイプ宣言
// void sendLineNotify(String message);
// void printAccel(Accel accel);

// //初期設定
// void setup() { 
//     Serial.begin(115200);

//     /* WIFIの設定 */
//     WiFi.begin(ssid, password);
//     // delay(10000); //10秒待つ
//     Serial.print("Connecting to WiFi...");
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(1000);
//         Serial.print(".");
//     }
//     Serial.println("Connected to WiFi");
//     // sendLineNotify("Arduino connected to WiFi!");

//     /* 加速度センサの設定 */
//     accel.begin(LIS3DH_ADDRESS);
//     accel.setClick(0, 0);                      // Disable Interrupt
//     accel.setRange(LIS3DH_RANGE_2_G);          // Full scale +/- 2G
//     accel.setDataRate(LIS3DH_DATARATE_10_HZ);  // Data rate = 10Hz
//     delay(100);
// }



// //メインループ
// void loop() {
//     /* 加速度センサの処理 */
//     accel.read();
//     //現在の加速度を取得
//     Accel nowAccel = {(double)accel.x, (double)accel.y, (double)accel.z};
//     // printAccel(nowAccel);
//     delay(10);

//     //直近の加速度をリストに追加
//     recentAccelList.push_back(nowAccel);
//     //リストの長さが最大長を超えたら先頭の要素を削除
//     if (recentAccelList.size() > MAX_RECENT_ACCEL) {
//         recentAccelList.pop_front();
//     }
//     //加速度リスト中の平均値を計算
//     Accel avgAccel = {0, 0, 0};
//     for (Accel accel : recentAccelList) {
//         avgAccel.x += accel.x;
//         avgAccel.y += accel.y;
//         avgAccel.z += accel.z;
//     }
//     avgAccel.x /= recentAccelList.size();
//     avgAccel.y /= recentAccelList.size();
//     avgAccel.z /= recentAccelList.size();

//     //表示
//     // printAccel(avgAccel);

//     //現在値と平均値の傾き
//     Accel slopeAccel = {nowAccel.x / avgAccel.x, nowAccel.y / avgAccel.y, nowAccel.z / avgAccel.z};
//     printAccel(slopeAccel);

//     //入水時の波による加速度の変化を検知
//     // if (abs(accel.x_g) > 0.7 || abs(accel.y_g) > 0.7) {
//     //     sendLineNotify("お子さんが浴槽に入水しています！！！！！");
//     //     delay(10000);
//     // }
// }


// //加速度を表示する関数
// void printAccel(Accel accel) {
//     Serial.print("X: " + String(accel.x));
//     Serial.print(", ");
//     Serial.print("Y: " + String(accel.y));
//     Serial.print(", ");
//     Serial.print("Z: " + String(accel.z));
//     Serial.println("");
// }




// // Line Notifyに通知を送る関数
// void sendLineNotify(String message) {
//     WiFiClientSecure client;
//     client.setInsecure(); //証明書無視??????????
//     if (!client.connect(server, 443)) {
//         Serial.println("Connection failed");
//         return;
//     }

//     String data = "message=" + message;
//     client.println("POST /api/notify HTTP/1.1");
//     client.println("Host: notify-api.line.me");
//     client.println("Authorization: Bearer " + String(lineToken));
//     client.println("Content-Type: application/x-www-form-urlencoded");
//     client.print("Content-Length: ");
//     client.println(data.length());
//     client.println();
//     client.println(data);

//     Serial.println("Notification sent: " + message);
// }