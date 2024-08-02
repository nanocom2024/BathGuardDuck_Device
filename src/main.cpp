#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WiFiClientSecure.h>
#include "web_server.h"
// #include "esp32_ap_touch.h"
#include <Adafruit_LIS3DH.h>
#define LIS3DH_ADDRESS 0x19
const char WIFI_SSID[] = "Leafony_ESP32-AP";
const char WIFI_PASSWORD[] = "password";
// WiFiServer server(80);
Adafruit_LIS3DH accel = Adafruit_LIS3DH();

const char* ssid = "04F-Pluslab";
const char* password = "bearspooh";
const char* lineToken = ""; // ここにLine Notifyのトークンを入れる
const char* server = "notify-api.line.me";

//プロトタイプ宣言
void sendLineNotify(String message);

void setup() { 
    Serial.begin(115200);

    /* WIFIの設定 */
    WiFi.begin(ssid, password);
    // delay(10000); //10秒待つ
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
    // sendLineNotify("Arduino connected to WiFi!");

    /* 加速度センサの設定 */
    accel.begin(LIS3DH_ADDRESS);
    accel.setClick(0, 0);                      // Disable Interrupt
    accel.setRange(LIS3DH_RANGE_2_G);          // Full scale +/- 2G
    accel.setDataRate(LIS3DH_DATARATE_10_HZ);  // Data rate = 10Hz
    delay(100);
}

void loop() {
    /* 加速度センサの処理 */
    accel.read();
    Serial.print("X [g] = " + String(accel.x_g));
    Serial.print(", ");
    Serial.print("Y [g] = " + String(accel.y_g));
    Serial.print(", ");
    Serial.print("Z [g] = " + String(accel.z_g));
    Serial.println("");
    delay(100);

    if (abs(accel.x_g) > 0.7 || abs(accel.y_g) > 0.7) {
        sendLineNotify("めっちゃ揺れてる！！！！！！！！！！！！");
        delay(10000);
    }
}

// Line Notifyに通知を送る関数
void sendLineNotify(String message) {
    WiFiClientSecure client;
    client.setInsecure(); //証明書無視??????????
    if (!client.connect(server, 443)) {
        Serial.println("Connection failed");
        return;
    }

    String data = "message=" + message;
    client.println("POST /api/notify HTTP/1.1");
    client.println("Host: notify-api.line.me");
    client.println("Authorization: Bearer " + String(lineToken));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);

    Serial.println("Notification sent: " + message);
}