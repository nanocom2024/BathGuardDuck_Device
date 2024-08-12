#include "selectMainFile.h"
#ifdef SELECT_BLE
#include "accelDef.h"
#include "bleDef.h"

void setup() {
  Serial.begin(115200);

  /* BLEの設定 */
  bleSetUp();
  Serial.println("Waiting a client connection to notify...");

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

  //stdをBLEで送信
  sendStdToBLE(std);

  //遅延
  delay(10);
}
#endif

