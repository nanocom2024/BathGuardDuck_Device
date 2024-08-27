const char* server = "bath-guard-duck-web.vercel.app";
#include <WiFiClientSecure.h>

const char* ssid = "";
const char* password = "";
// const char* lineToken = ""; // ここにLine Notifyのトークンを入れる
const char* notifyMes = "めっちゃ揺れてる！！！！！！！！！！！！";

// プロトタイプ宣言
//  void sendLineNotify(String message);
void sendNotification();

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