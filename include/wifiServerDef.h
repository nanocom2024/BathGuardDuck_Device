const char* server = "bath-guard-duck-web.vercel.app";
#include <WiFiClientSecure.h>
#include "../include/webServer.h"
WiFiServer localServer(80);

const char LOCAL_WIFI_SSID[] = "Leafony_ESP32-AP";
const char LOCAL_WIFI_PASSWORD[] = "password";

//設定用変数
String savedUserid = "";
String savedSsid = "";
String savedPassword = "";

//プロトタイプ宣言
// void sendLineNotify(String message);
// void sendNotification();

//初期設定
void localServerSetUp() {
    // サーバの設定
    WiFi.softAP(LOCAL_WIFI_SSID, LOCAL_WIFI_PASSWORD);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    localServer.begin();
    Serial.println("Server started");
}

void wifiSetUp() {
    // WiFiの設定
    WiFi.begin(savedSsid, savedPassword);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
}

//userIDを取得する関数
String getUserid() {
    return savedUserid;
}

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
                                savedUserid = allBody.substring(index + 7, allBody.indexOf("&", index));
                            }
                            index = allBody.indexOf("ssid=");
                            if (index > -1) {
                                savedSsid = allBody.substring(index + 5, allBody.indexOf("&", index));
                            }
                            index = allBody.indexOf("password=");
                            if (index > -1) {
                                //最後の文字まで取り出す
                                savedPassword = allBody.substring(index + 9, allBody.length());
                            }

                            //設定完了
                            Serial.println("userid: " + savedUserid);
                            Serial.println("ssid: " + savedSsid);
                            Serial.println("password: " + savedPassword);
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

//POSTする関数
void sendNotification() { //String message
    WiFiClientSecure client;
    client.setInsecure(); //証明書無視??????????
    if (!client.connect(server, 443)) {
        Serial.println("Connection failed");
        return;
    }

    //POSTするjsonデータ
    String data = "{\"state\":\"ENTER_WATER\", \"token\":\">>>SET TOKEN HERE<<<\", \"email\":\"" + savedUserid + "\"}"; 
    client.println("POST /api/v1/send-notification HTTP/1.1"); //HTTPリクエスト
    client.println("Host: " + String(server)); //Hostの設定
    client.println("Content-Type: application/json"); // JSON形式のContent-Type
    client.print("Content-Length: ");
    client.println(data.length());
    client.println(); //ヘッダ終了
    client.println(data); //メッセージ本体

    Serial.println("Notification sent: " + data);
}