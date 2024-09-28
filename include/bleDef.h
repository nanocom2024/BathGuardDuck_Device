#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic* pCharacteristic;
bool deviceConnected = false;
const char* detectionStr = "detection!";

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) { deviceConnected = true; };

  void onDisconnect(BLEServer* pServer) { deviceConnected = false; }
};

// BLE初期化関数
void bleSetUp() {
  // Create the BLE Device
  BLEDevice::init("MyESP32");

  // Create the BLE Server
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ |
                               BLECharacteristic::PROPERTY_WRITE |
                               BLECharacteristic::PROPERTY_NOTIFY |
                               BLECharacteristic::PROPERTY_INDICATE);

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
}

// BLEに加速度の標準偏差を送信する関数
void sendStdToBLE(double std) {
  Serial.println(std);
  // BLEに接続されていない場合は送信しない
  if (!deviceConnected) return;

  // uint8_tに変換
  //   int intStd = (int)(std / 600);  // 0~60あたり
  int intStd = (int)(std);  // 0~60あたり  / 600
  uint8_t uint8Std = (uint8_t)intStd;
  // Serial.println("std: " + String(uint8Std));

  /*BLEで送信*/
  // 標準偏差が5以上の場合は検知として通知
  //   if (intStd >= 5) {
  //     // 文字列を送信
  //     pCharacteristic->setValue((uint8_t*)detectionStr,
  //     strlen(detectionStr)); pCharacteristic->notify(); return;
  //   }

  // 標準偏差を送信
  pCharacteristic->setValue(&uint8Std, 10);
  pCharacteristic->notify();
  // pCharacteristic->indicate();
  //  Serial.printf("*** NOTIFY: %d ***\n", value);
}