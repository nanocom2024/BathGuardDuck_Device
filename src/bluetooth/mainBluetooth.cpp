#include "selectMainFile.h"
#ifdef SELECT_Bluetooth
#include "BluetoothSerial.h"
#include "accelDef.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200); // シリアルモニタ用

  /* Bluetoothの設定 */
  SerialBT.begin("ESP32_Test"); // Bluetoothデバイス名を設定
  Serial.println("The device started, now you can pair it with Bluetooth!");
  /* 加速度センサの設定 */
  accelSetUp();
  delay(100);
}

// 変数をグローバルに宣言
unsigned long startTimeBelow = 0; // 閾値を下回った時間の計測
unsigned long startTimeAbove = 0; // 閾値を上回った時間の計測
unsigned long startTimeWave = 0;
bool isBelow = false; // zが閾値を下回っているかどうか
bool isAbove = false; // zが閾値を上回っているかどうか
bool isWave = false;
int angle = 14000; // 水上に浮いているかどうかの閾値

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
  double stdX = getAccelStdX(avgAccel);
  double stdY = getAccelStdY(avgAccel);
  double stdZ = getAccelStdZ(avgAccel);

  //Bluetoothで送信
  SerialBT.print(nowAccel.z);
  SerialBT.print(',');
  SerialBT.println(std);

    // 陸上に設置されているかどうかの判定
    if (nowAccel.z < angle) {
        if (!isBelow) {
            // 初めて閾値を下回ったら時間を記録
            startTimeBelow = millis();
            isBelow = true;
        } else if (millis() - startTimeBelow >= 10000) {
            // 閾値を下回り続けて2分が経過したら「陸上に設置されている」と判定
            SerialBT.println("陸上に設置されている");
            // 必要な処理をここに記述
        }
    } else {
        // 閾値を超えたらリセット
        isBelow = false;
    }

    if (nowAccel.z >= angle) {
        if (!isAbove) {
        startTimeAbove = millis();
        isAbove = true;
        } else if (millis() - startTimeAbove >= 10000) { // 2分に変更
        // 閾値を超え続けて2分が経過したら「水上に浮いている」と判定
            SerialBT.println("水上に浮いている");
            if (std > 1000) {
                startTimeWave = millis();
                isWave = true;
            } else if (millis() - startTimeWave >= 9000 && isWave) {
                SerialBT.println("溺れ検知");
                isWave = false; // リセット
                delay(5000);
            }
        }
    } else {
    isAbove = false;
    }

  //遅延
  delay(20);
}
#endif