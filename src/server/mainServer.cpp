#include "selectMainFile.h"
#ifdef SELECT_SERVER
#include <list>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "webServer.h"
#include <Adafruit_LIS3DH.h>

const char WIFI_SSID[] = "Leafony_ESP32-AP";
const char WIFI_PASSWORD[] = "password";
WiFiServer server(80);
Adafruit_LIS3DH accel = Adafruit_LIS3DH();
#define LIS3DH_ADDRESS 0x19

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
void printAccel(Accel accel);
String getAccelStr(Accel accel);
void printAccel(Accel accel);

void setup() {
    /* wifi設定 */
    Serial.begin(115200);
    Serial.println("Wi-Fi & Touch Sensor Test");
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();
    Serial.println("Server started");

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

    //現在値と平均値の傾き
    Accel slopeAccel = {nowAccel.x / avgAccel.x, nowAccel.y / avgAccel.y, nowAccel.z / avgAccel.z};
    String accelStr = getAccelStr(slopeAccel);
    // Serial.println(accelStr);
    printAccel(slopeAccel);

    int ret;
    int refreshTime = 1;                                // HTML page refresh time (sec)
    WiFiClient client = server.available();             // listen for incoming clients

    if (client) {                                       // if you get a client
        String currentLine = "";                        // make a String to hold incoming data from the client
        while (client.connected()) {                    // loop while the client's connected
            if (client.available()) {                   // if there's bytes to read from the client
                char byteData = client.read();          // read a byte, then
                if (byteData == '\n') {                 // if the byte is newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // That's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        htmlTouchSensorMain(            // send html contents
                            client,
                            refreshTime,
                            WiFi.softAPIP(),
                            accelStr
                            );
                        break;                          // break out of the while loop:
                    } else {                            // if you got a newline
                        // check if the data from client contains "GET /?INT="
                        ret = currentLine.indexOf("GET /?INT=");
                        if (ret > -1) {     // if found,
                            // extract the number corresponding to required page refresh period from currentLine
                            // and cast it to String, and cast it again to int.
                            // Then, assign to refreshTime
                            refreshTime = String(currentLine.charAt(ret + 10)).toInt();
                            Serial.print("refresh time updated :");
                            Serial.print(refreshTime);
                            Serial.println("s");
                        }
                        currentLine = "";   // clear the current Line to store new incoming data.
                    }
                } else if (byteData != '\r') {          // if you got anything else but a carriage return character,
                    currentLine += String(byteData);    // add it to the end of the currentLine
                }
            }
        }
    }
}


String getAccelStr(Accel accel) {
    String accelStr = "X: " + String(accel.x) + ", Y: " + String(accel.y) + ", Z: " + String(accel.z);
    return accelStr;
}

//加速度を表示する関数
void printAccel(Accel accel) {
    Serial.print("X: " + String(accel.x));
    Serial.print(", ");
    Serial.print("Y: " + String(accel.y));
    Serial.print(", ");
    Serial.print("Z: " + String(accel.z));
    Serial.println("");
}
#endif