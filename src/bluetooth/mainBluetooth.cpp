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
unsigned long startTimeAbove = 0; // 15000を超えた時間の計測
unsigned long startTimeBelow = 0; // 12500を下回った時間の計測
bool isAbove15000 = false; // zが15000を超えているかどうか
bool isBelow12500 = false; // zが12500を下回っているかどうか


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
  // SerialBT.print(nowAccel.x);
  // SerialBT.print(',');
  // SerialBT.print(nowAccel.y);
  // SerialBT.print(',');
  // SerialBT.print(nowAccel.z);
  // SerialBT.print(',');
  // SerialBT.print(std);
  // SerialBT.print(',');
  // SerialBT.print(stdX);
  // SerialBT.print(',');
  // SerialBT.print(stdY);
  // SerialBT.print(',');
  // SerialBT.println(stdZ);
  
  // Serial.print(">avg:");
  // Serial.println(std);

// 水上に浮いているかどうかの判定
    if (nowAccel.z > 15000) {
        if (!isAbove15000) {
            // 初めて15000を超えたら時間を記録
            startTimeAbove = millis();
            isAbove15000 = true;
        } else if (millis() - startTimeAbove >= 3000) {
            // 15000を超え続けて2分が経過したら「水上に浮いている」と判定
            SerialBT.println("水上に浮いている");
            // 必要な処理をここに記述
        }
    } else {
        // 15000を下回ったらリセット
        isAbove15000 = false;
    }

    // 陸上に設置されているかどうかの判定
    if (nowAccel.z < 12500) {
        if (!isBelow12500) {
            // 初めて12500を下回ったら時間を記録
            startTimeBelow = millis();
            isBelow12500 = true;
        } else if (millis() - startTimeBelow >= 3000) {
            // 12500を下回り続けて2分が経過したら「陸上に設置されている」と判定
            SerialBT.println("陸上に設置されている");
            // 必要な処理をここに記述
        }
    } else {
        // 12500を超えたらリセット
        isBelow12500 = false;
    }

  //遅延
  delay(20);
}
#endif