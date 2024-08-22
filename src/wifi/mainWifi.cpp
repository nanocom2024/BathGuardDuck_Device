#include "selectMainFile.h"
#ifdef SELECT_WIFI
#include "accelDef.h"
// #include "wifiDefLine.h"
#include "wifiDefFire.h"

void setup() { 
    Serial.begin(115200);

    /* WIFIの設定 */
    wifiSetUp(ssid, password);

    /* 加速度センサの設定 */
    accelSetUp();
    delay(100);
}

void loop() {
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

    //標準偏差が5以上の場合はLine Notifyで通知
    if (intStd >= 5) {
        // sendLineNotify(notifyMes);
        sendFirebase(sendMes);
        delay(10000);
    }
}

// Line Notifyに通知を送る関数
// void sendLineNotify(String message) {
//     WiFiClientSecure client;
//     client.setInsecure(); //証明書無視??????????
//     if (!client.connect(server, 443)) {
//         Serial.println("Connection failed");
//         return;
//     }


//     // HTTPリクエストを送信
//     String data = "message=" + message;
//     client.println("POST /api/notify HTTP/1.1");
//     client.println("Host: notify-api.line.me");
//     client.println("Authorization: Bearer " + String(lineToken));
//     client.println("Content-Type: application/x-www-form-urlencoded");
//     client.print("Content-Length: ");
//     client.println(data.length());
//     client.println();
//     client.println(data);

//     Serial.println("Notification sent: " + message);
// }

// FirebaseにPOSTを送る関数
void sendFirebase(String message) {
    WiFiClientSecure client;
    client.setInsecure(); //証明書無視??????????
    if (!client.connect(server, 443)) {
        Serial.println("Connection failed");
        return;
    }

    // JSONデータを作成
    String jsonData = "{\"message\":\"" + message + "\"}";
    
    // HTTPリクエストを送信
    client.println("POST " + String(firebasePath) + " HTTP/1.1");
    client.println("Host: " + String(firebaseHost));
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(jsonData.length());
    client.println("Connection: close");
    client.println();
    client.println(jsonData);

    Serial.println("Notification sent: " + message);
}
#endif