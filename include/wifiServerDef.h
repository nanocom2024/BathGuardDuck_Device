const char* server = "bath-guard-duck-web.vercel.app";
#include <WiFiClientSecure.h>
#include "../include/webServer.h"
WiFiServer localServer(80);

const char* ssid = "";
const char* password = "";
const char* notifyMes = "めっちゃ揺れてる！！！！！！！！！！！！";

const char LOCAL_WIFI_SSID[] = "Leafony_ESP32-AP";
const char LOCAL_WIFI_PASSWORD[] = "password";

//プロトタイプ宣言
// void sendLineNotify(String message);
void sendNotification();

//初期設定
void wifiServerSetUp() {
    // WiFiの設定
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");

    // サーバの設定
    WiFi.softAP(LOCAL_WIFI_SSID, LOCAL_WIFI_PASSWORD);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    localServer.begin();
    Serial.println("Server started");
}

/* wifi設定 */
// WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
// IPAddress myIP = WiFi.softAPIP();
// Serial.print("AP IP address: ");
// Serial.println(myIP);
// server.begin();
// Serial.println("Server started");


//サーバを立ち上げる関数
bool runningServer() {
    int ret;
    int refreshTime = 0;                                // HTML page refresh time (sec)
    WiFiClient client = localServer.available();              // 受信クライアントをリッスンする
    bool isSetup = false; //設定が完了したか

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
                            isSetup
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

                            //設定完了
                            Serial.println("userid: " + userid);
                            Serial.println("ssid: " + ssid);
                            Serial.println("password: " + password);
                            isSetup = true;

                            //表示する
                            htmlTouchSensorMain(
                                client,
                                refreshTime,
                                WiFi.softAPIP(),
                                isSetup
                            );
                            break; //設定が完了したのでループを抜ける
                        }
                        currentLine = "";   // 新しい受信データを保存するために、現在のLineをクリアする。
                    }
                } else if (byteData != '\r') {          // \r以外の文字があれば、
                    currentLine += String(byteData);    // それをカレント行の最後に追加する
                }
            }
        }
    }
    return isSetup;
}