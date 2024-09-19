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
    // printAccel(slopeAccel);

    int ret;
    int refreshTime = 0;                                // HTML page refresh time (sec)
    WiFiClient client = server.available();              // 受信クライアントをリッスンする

    if (client) {                                       // クライアントを獲得したら
        String currentLine = "";                        // クライアントからの受信データを格納する変数を作成する
        while (client.connected()) {                    // クライアントが接続している間ループする
            if (client.available()) {                   // クライアントから読み込むバイトがある場合
                char byteData = client.read();          // バイトを読み取り
                if (byteData == '\n') {                 // バイトが改行の場合
                    // 現在の行が空白なら、改行文字が2つ並んでいることになるので、
                    // これでクライアントのHTTPリクエストは終了となり、レスポンスを送信する
                    if (currentLine.length() == 0) {
                        htmlTouchSensorMain(            // htmlコンテンツを送信する
                            client,
                            refreshTime,
                            WiFi.softAPIP(),
                            accelStr,
                            false
                        );
                        break;                          // whileループから抜け出す
                    } else {                            // 改行があれば
                        // クライアントからのデータに 「GET /?INT=」が含まれているかチェックする
                        ret = currentLine.indexOf("GET /?INT="); ///?INT=はURLのパラメータ?
                        if (ret > -1) {     // もし見つかれば
                            // currentLineから必要なページ更新期間に対応する数値を取り出し、
                            // それをStringにキャストし、再びintにキャストする。
                            // 次に、refreshTimeに代入する。
                            refreshTime = String(currentLine.charAt(ret + 10)).toInt();
                            Serial.print("refresh time updated :");
                            Serial.print(refreshTime);
                            Serial.println("s");
                        }

                        //POSTがあるかチェック
                        String userid = "";
                        String ssid = "";
                        String password = "";
                        Serial.println("nu: " + currentLine);
                        ret = currentLine.indexOf("POST"); //userid=nu&ssid=nu&password=nu
                        if (ret > -1) {
                            Serial.println("POST request");
                            //bodyのデータを読み込む
                            String allBody = "";
                            while (client.available()) {
                                byteData = client.read();
                                allBody += String(byteData);
                            }

                            //allBodyからuserid、ssid、passwordを取り出す
                            int index = allBody.indexOf("userid=");
                            if (index > -1) {
                                userid = allBody.substring(index + 7, allBody.indexOf("&", index));
                            }
                            index = allBody.indexOf("ssid=");
                            if (index > -1) {
                                ssid = allBody.substring(index + 5, allBody.indexOf("&", index));
                            }
                            index = allBody.indexOf("password=");
                            if (index > -1) {
                                //最後の文字まで取り出す
                                password = allBody.substring(index + 9, allBody.length());
                            }
                            Serial.println("userid: " + userid);
                            Serial.println("ssid: " + ssid);
                            Serial.println("password: " + password);
                            htmlTouchSensorMain(            // htmlコンテンツを送信する
                                client,
                                refreshTime,
                                WiFi.softAPIP(),
                                accelStr,
                                true
                            );
                        }
                        currentLine = "";   // 新しい受信データを保存するために、現在のLineをクリアする。
                    }
                } else if (byteData != '\r') {          // \r以外の文字があれば、
                    currentLine += String(byteData);    // それをカレント行の最後に追加する
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