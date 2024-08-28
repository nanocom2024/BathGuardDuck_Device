#include "selectMainFile.h"
#ifdef SELECT_PRODUCT
#include "accelDef.h"
#include "wifiServerDef.h"

void setup() { 
    Serial.begin(115200);

    /* WIFIの設定 */
    wifiServerSetUp(ssid, password);

    /* 加速度センサの設定 */
    accelSetUp();
    delay(100);
}


bool isSetup = false;
void loop() {
    /* 設定用サーバの立ち上げ */
    isSetup = runningServer();

    if (!isSetup) return;

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