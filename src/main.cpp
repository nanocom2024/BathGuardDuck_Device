#include <Arduino.h>

//初期化
void setup() {
  Serial.begin(115200);
  delay(10);
}

void loop() {
  Serial.println("Hello World");
  delay(1000);
}
