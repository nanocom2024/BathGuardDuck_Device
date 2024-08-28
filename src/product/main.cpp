#include "selectMainFile.h"
#ifdef SELECT_PRODUCT
#include "accelDef.h"
#include "wifiServerDef.h"

const char WIFI_SSID[] = "Leafony_ESP32-AP";
const char WIFI_PASSWORD[] = "password";


void setup() { 
    Serial.begin(115200);

    /* ローカルサーバの設定 */
    localServerSetUp();

    /* 加速度センサの設定 */
    accelSetUp();
    delay(100);
}

//初期設定用フラグ
bool isSetup = false; //ローカルサーバからの設定が完了したか
bool isConnectedToWifi = false; //WiFiに接続したか

void loop() {
    /* 設定用サーバの立ち上げ */
    if (!isSetup) {
        isSetup = runningServer();
        return;
    }

    /* 受け取った設定のWiFiに接続 */
    wifiSetUp();

    /* 加速度センサの処理 */
    //現在の加速度を取得
    Accel nowAccel = getNowAccel();
    // printAccel(nowAccel);

    //直近の加速度をリストに追加
    addAccelToList(nowAccel);

    //加速度リスト中の平均値を計算
    Accel avgAccel = getAvgAccel();

    //分散と標準偏差計算
    double std = getAccelStd(avgAccel);
    int intStd = (int)(std / 600); //0~60あたり
    Serial.println("std: " + String(intStd));

    //標準偏差が5以上の場合は通知
    if (intStd >= 5) {
        sendNotification();
        clearAccelList();
        delay(10000);
    }
}

//POSTする関数
void sendNotification() { //String message
    WiFiClientSecure client;
    client.setInsecure(); //証明書無視??????????
    if (!client.connect(server, 443)) {
        Serial.println("Connection failed");
        return;
    }

    String data = "{\"state\":\"ENTER_WATER\", \"token\":\">>>SET TOKEN HERE<<<\"}";  // 固定のJSONデータ
    client.println("POST /api/v1/send-notification HTTP/1.1"); //HTTPリクエスト
    client.println("Host: " + String(server)); //Hostの設定
    client.println("Content-Type: application/json"); // JSON形式のContent-Type
    client.print("Content-Length: ");
    client.println(data.length());
    client.println(); //ヘッダ終了
    client.println(data); //メッセージ本体

    Serial.println("Notification sent: " + data);
}



#endif