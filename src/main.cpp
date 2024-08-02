#include <WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "04F-Pluslab";
const char* password = "bearspooh";
const char* lineToken = "42kBuQ8hVoDKT7hYyrRyyQC3hBni4LbaXLYpzWr2WNA";
const char* server = "notify-api.line.me";

void sendLineNotify(String message) {
    WiFiClientSecure client;
    client.setInsecure(); //証明書無視
    if (!client.connect(server, 443)) {
        Serial.println("Connection failed");
        return;
    }

    String data = "message=" + message;
    client.println("POST /api/notify HTTP/1.1");
    client.println("Host: notify-api.line.me");
    client.println("Authorization: Bearer " + String(lineToken));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);

    Serial.println("Notification sent: " + message);
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    delay(10000);
    Serial.print("Connecting to WiFi...");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
    
    sendLineNotify("Arduino connected to WiFi!");
}

void loop() {
  // Add your main code here
}




// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <WiFiAP.h>
// #include "web_server.h"
// // #include "esp32_ap_touch.h"
// #include <Adafruit_LIS3DH.h>
// #define LIS3DH_ADDRESS 0x19
// const char WIFI_SSID[] = "Leafony_ESP32-AP";
// const char WIFI_PASSWORD[] = "password";
// WiFiServer server(80);
// Adafruit_LIS3DH accel = Adafruit_LIS3DH();

// void setup() { 
//     Serial.begin(115200);
//     Serial.println("Wi-Fi & Touch Sensor Test");

//     /* WIFIの設定 */
//     // WiFi settings
//     WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
//     IPAddress myIP = WiFi.softAPIP();
//     Serial.print("AP IP address: ");
//     Serial.println(myIP);
//     // Wifi server start
//     server.begin();
//     Serial.println("Server started");

//     /* 加速度センサの設定 */
//     accel.begin(LIS3DH_ADDRESS);
//     accel.setClick(0, 0);                      // Disable Interrupt
//     accel.setRange(LIS3DH_RANGE_2_G);          // Full scale +/- 2G
//     accel.setDataRate(LIS3DH_DATARATE_10_HZ);  // Data rate = 10Hz
//     delay(100);
// }

// void loop() {
//     /* 加速度センサの処理 */
//     accel.read();
//     Serial.print("X [g] = " + String(accel.x_g));
//     Serial.print(", ");
//     Serial.print("Y [g] = " + String(accel.y_g));
//     Serial.print(", ");
//     Serial.print("Z [g] = " + String(accel.z_g));
//     Serial.println("");
//     delay(100);

//     /* 以下WIFI接続の処理 */
//     int ret;
//     int refreshTime = 1;                                // HTML page refresh time (sec)
//     WiFiClient client = server.available();             // listen for incoming clients

//     if (client) {                                       // if you get a client
//         String currentLine = "";                        // make a String to hold incoming data from the client
//         while (client.connected()) {                    // loop while the client's connected
//             if (client.available()) {                   // if there's bytes to read from the client
//                 char byteData = client.read();          // read a byte, then
//                 if (byteData == '\n') {                 // if the byte is newline character
//                     // if the current line is blank, you got two newline characters in a row.
//                     // That's the end of the client HTTP request, so send a response:
//                     if (currentLine.length() == 0) {
//                         String accelStr = 
//                         "X [g] = " + String(accel.x_g) +
//                         ", Y [g] = " + String(accel.y_g) +
//                         ", Z [g] = " + String(accel.z_g);

//                         htmlTouchSensorMain(            // send html contents
//                             client,
//                             refreshTime,
//                             WiFi.softAPIP(),
//                             accelStr
//                         );
//                         break;                          // break out of the while loop:
//                     } else {                            // if you got a newline
//                         // check if the data from client contains "GET /?INT="
//                         ret = currentLine.indexOf("GET /?INT=");
//                         if (ret > -1) {     // if found,
//                             // extract the number corresponding to required page refresh period from currentLine
//                             // and cast it to String, and cast it again to int.
//                             // Then, assign to refreshTime
//                             refreshTime = String(currentLine.charAt(ret + 10)).toInt();
//                             Serial.println("refresh time updated :");
//                             Serial.print(refreshTime);
//                             Serial.println ("s");
//                         }
//                         currentLine = "";   // clear the current Line to store new incoming data.
//                     }
//                 } else if (byteData != '\r') {          // if you got anything else but a carriage return character,
//                     currentLine += String(byteData);    // add it to the end of the currentLine
//                 }
//             }
//         }
//     }
// }



// //=====================================================================
// //  Leafony Platform sample sketch
// //     Platform     : ESP32
// //     Processor    : ESP32-WROOM-32
// //     Application  : ESP32 Touch Sensor
// //
// //     Leaf configuration
// //       (1) AP02A ESP MCU
// //       (2) AX08A 29pin header
// //
// //    (c) 2021 LEAFONY SYSTEMS Co., Ltd
// //    Released under the MIT license
// //    https://opensource.org/licenses/MIT
// //
// //      Rev.00 2021/04/01  First release
// //=====================================================================