#include "selectMainFile.h"
#ifdef SELECT_PRODUCT
#include "accelDef.h"
#include "wifiServerDef.h"

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

//加速度センサ用の変数をグローバルに宣言
unsigned long startTimeBelow = 0; // 閾値を下回った時間の計測
unsigned long startTimeAbove = 0; // 閾値を上回った時間の計測
unsigned long startTimeWave = 0;
bool isBelow = false; // zが閾値を下回っているかどうか
bool isAbove = false; // zが閾値を上回っているかどうか
bool isWave = false;
int angle = 14000; // 水上に浮いているかどうかの閾値

//ダックの状態
enum DuckState {
    ON_LAND,
    ON_WATER,
    DROWNING
};

//通知用のダックの状態
String DuckNotifStateList[] = {
    "SLEEP", //ON_LAND
    "CHILD_ENTER", //ON_WATER
    "CHILD_DETECTION" //DROWNING
};

//現在のダックの状態
DuckState nowDuckState = ON_LAND;

//引数の状態に遷移する関数
void changeDuckState(DuckState targetState);

void loop() {
    /* 設定用サーバの立ち上げ */
    if (!isSetup) {
        isSetup = runningServer();
        Serial.print(".");
        return;
    }

    /* 受け取った設定のWiFiに接続 */
    if (!isConnectedToWifi) {
        wifiSetUp();
        isConnectedToWifi = true;
        localServerEnd();
    }

    /* 加速度センサの処理 */
    //現在の加速度を取得
    Accel nowAccel = getNowAccel();

    //直近の加速度をリストに追加
    addAccelToList(nowAccel);

    //加速度リスト中の平均値を計算
    Accel avgAccel = getAvgAccel();

    //分散と標準偏差計算
    double std = getAccelStd(avgAccel);

    //加速度センサの値をシリアルモニタに表示
    Serial.println("std: " + String(std));

    // 陸上に設置されているかどうかの判定
    if (nowAccel.z < angle) {
        if (!isBelow) {
            // 初めて閾値を下回ったら時間を記録
            startTimeBelow = millis();
            isBelow = true;
        } else if (millis() - startTimeBelow >= 10000) {
            // 閾値を下回り続けて10秒が経過したら「陸上に設置されている」と判定
            changeDuckState(ON_LAND);
            isWave = false; // リセット
            // Serial.println("陸上に設置されている");
        }
    } else {
        // 閾値を超えたらリセット
        isBelow = false;
    }

    if (nowAccel.z >= angle) { //傾きが一定以上になる(陸上推定)
        if (!isAbove) {
        startTimeAbove = millis();
        isAbove = true;
        } else if (millis() - startTimeAbove >= 10000) { // 10秒に変更(TODO: 2分に変更(揺れが落ち着くまで陸上判定にしておくため))
        // 閾値を超え続けて30秒が経過したら「水上に浮いている」と判定
            changeDuckState(ON_WATER);
            // Serial.println("水上に浮いている");
            if (std > 1000) {
                startTimeWave = millis();
                isWave = true;
            } else if (millis() - startTimeWave >= 9000 && isWave) {
                //「溺れている」と判定
                changeDuckState(DROWNING);
                // Serial.println("溺れている");
                isWave = false; // リセット
                delay(30000); //30秒間表示
                //TODO: 溺れている状態から水上に戻らないバグを修正する
            }
        }
    } else {
        isAbove = false;
    }
    //遅延
    delay(20);
}

//引数の状態に遷移する関数
void changeDuckState(DuckState targetState) {
    //現在の状態と対象の状態を比較
    if (nowDuckState != targetState) {
        nowDuckState = targetState; //状態切り替え
        //遷移時のみ通知を飛ばす
        sendNotification(
            DuckNotifStateList[static_cast<int>(nowDuckState)]
        );
    }
}

//POSTする関数
// void sendNotification() { //String message
//     WiFiClientSecure client;
//     client.setInsecure(); //証明書無視??????????
//     if (!client.connect(server, 443)) {
//         Serial.println("Connection failed");
//         return;
//     }

//     String data = "{\"state\":\"ENTER_WATER\", \"token\":\">>>SET TOKEN HERE<<<\"}";  // 固定のJSONデータ
//     client.println("POST /api/v1/send-notification HTTP/1.1"); //HTTPリクエスト
//     client.println("Host: " + String(server)); //Hostの設定
//     client.println("Content-Type: application/json"); // JSON形式のContent-Type
//     client.print("Content-Length: ");
//     client.println(data.length());
//     client.println(); //ヘッダ終了
//     client.println(data); //メッセージ本体

//     Serial.println("Notification sent: " + data);
// }

#endif