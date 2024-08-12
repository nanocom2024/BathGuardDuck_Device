
#include <Adafruit_LIS3DH.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <list>

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
uint8_t value = 0;
Adafruit_LIS3DH accel = Adafruit_LIS3DH();
#define LIS3DH_ADDRESS 0x19

//加速度の構造体
struct Accel {
    double x;
    double y;
    double z;
};
//直近の加速度のリスト
std::list<Accel> recentAccelList;
//リストの最大長
const int MAX_RECENT_ACCEL = 20;

//プロトタイプ宣言
void printAccel(Accel accel);
void sendStdToBLE(double std);

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


void setup() {
  Serial.begin(115200);

  /* BLEの設定 */
  // Create the BLE Device
  BLEDevice::init("MyESP32");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ   |
      BLECharacteristic::PROPERTY_WRITE  |
      BLECharacteristic::PROPERTY_NOTIFY |
      BLECharacteristic::PROPERTY_INDICATE
    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  /* 加速度センサの設定 */
  accel.begin(LIS3DH_ADDRESS);
  accel.setClick(0, 0);                      // Disable Interrupt
  accel.setRange(LIS3DH_RANGE_2_G);          // Full scale +/- 2G
  accel.setDataRate(LIS3DH_DATARATE_10_HZ);  // Data rate = 10Hz
  delay(100);
}

void loop() {
  /* 加速度センサの処理 */
  accel.read();
  //現在の加速度を取得
  Accel nowAccel = {(double)accel.x, (double)accel.y, (double)accel.z};
  // printAccel(nowAccel);

  //直近の加速度をリストに追加
  recentAccelList.push_back(nowAccel);
  //リストの長さが最大長を超えたら先頭の要素を削除
  if (recentAccelList.size() > MAX_RECENT_ACCEL) {
      recentAccelList.pop_front();
  }
  //加速度リスト中の平均値を計算
  Accel avgAccel = {0, 0, 0};
  for (Accel accel : recentAccelList) {
      avgAccel.x += accel.x;
      avgAccel.y += accel.y;
      avgAccel.z += accel.z;
  }
  avgAccel.x /= recentAccelList.size();
  avgAccel.y /= recentAccelList.size();
  avgAccel.z /= recentAccelList.size();

  //表示
  // printAccel(avgAccel);

  //現在値と平均値の傾き
  //揺れが少ないほど1に近づく
  // Accel slopeAccel = {nowAccel.x / avgAccel.x, nowAccel.y / avgAccel.y, nowAccel.z / avgAccel.z};
  // printAccel(slopeAccel);

  //分散と標準偏差計算
  double var = 0;
  for (Accel accel : recentAccelList) {
      var += pow(accel.x - avgAccel.x, 2);
      var += pow(accel.y - avgAccel.y, 2);
      var += pow(accel.z - avgAccel.z, 2);
  }
  var /= recentAccelList.size();
  double std = sqrt(var);

  //stdをBLEで送信
  sendStdToBLE(std);

  //遅延
  delay(10);
}

//加速度を表示する関数
void printAccel(Accel accel) {
    Serial.print("X: " + String(accel.x));
    Serial.print(", ");
    Serial.print("Y: " + String(accel.y));
    Serial.print(", ");
    Serial.print("Z: " + String(accel.z));
    Serial.println("");
}

//BLEに加速度の標準偏差を送信する関数
void sendStdToBLE(double std) {
    //BLEに接続されていない場合は送信しない
    if (!deviceConnected) return;

    //uint8_tに変換
    int intStd = (int)(std / 600); //0~60あたり
    uint8_t uint8Std = (uint8_t)intStd;
    Serial.println("std: " + String(uint8Std));
    
    /*BLEで送信*/
    //標準偏差が5以上の場合は検知として通知
    if (intStd >= 5) {
      String detectionStr = "detection!";
      const char* charArr = detectionStr.c_str();
      //文字列を送信
      pCharacteristic->setValue((uint8_t*)charArr, strlen(charArr));
      pCharacteristic->notify();
      return;
    }

    //標準偏差を送信
    pCharacteristic->setValue(&uint8Std, 1);
    pCharacteristic->notify();
    //pCharacteristic->indicate();
    // Serial.printf("*** NOTIFY: %d ***\n", value);
}







/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

    Create a BLE server that, once we receive a connection, will send periodic notifications.
    The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
    And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

    The design of creating the BLE server is:
    1. Create a BLE Server
    2. Create a BLE Service
    3. Create a BLE Characteristic on the Service
    4. Create a BLE Descriptor on the characteristic
    5. Start the service.
    6. Start advertising.

    A connect hander associated with the server starts a background task that performs notification
    every couple of seconds.
*/

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/