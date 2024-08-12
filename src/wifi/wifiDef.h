const char* server = "notify-api.line.me";
#include <WiFiClientSecure.h>

const char* ssid = "ctc-g-b4e19a";
const char* password = "393becfe5cc64";
const char* lineToken = "M9j0iUaJA1T2uLuF91vC4YWqnjAjhetlRwdytDlv6Mj"; // ここにLine Notifyのトークンを入れる
const char* notifyMes = "めっちゃ揺れてる！！！！！！！！！！！！";

//プロトタイプ宣言
void sendLineNotify(String message);

void wifiSetUp(const char* ssid, const char* password) {
    // WiFiの設定
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
}