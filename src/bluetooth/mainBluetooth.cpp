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
  double stdZ = getAccelStdZ(avgAccel);

  //Bluetoothで送信
  SerialBT.print(nowAccel.x);
  SerialBT.print(',');
  SerialBT.print(nowAccel.y);
  SerialBT.print(',');
  SerialBT.print(nowAccel.z);
  SerialBT.print(',');
  SerialBT.println(std);
  SerialBT.print(',');
  SerialBT.println(stdZ);
  
  // Serial.print(">avg:");
  // Serial.println(std);

  //遅延
  delay(20);
}
#endif