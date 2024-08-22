const char* server = "notify-api.line.me";
#include <WiFiClientSecure.h>

const char* ssid = "";
const char* password = "";
const char* firebaseHost = "your-project-id.firebaseio.com";  // Firebaseのホスト名
const char* firebasePath = "/messages.json"; 
const char* sendMes = "めっちゃ揺れてる！！！！！！！！！！！！";

//プロトタイプ宣言
void sendFirebase(String message);

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