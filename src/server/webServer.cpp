#include "selectMainFile.h"
#ifdef SELECT_SERVER
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "webServer.h"
#include <Adafruit_LIS3DH.h>

static int printMsgIfTouched(WiFiClient client, uint16_t touchData, String htmlMsg) {
    uint16_t touchTemp;
    const uint16_t THRESHOLD = 20;

    client.print(htmlMsg);
    for (touchTemp = 0; touchTemp <= touchData / 5; touchTemp++) {
        client.print("*");
    }
    client.print(":" + String(touchData));
    if (touchData > THRESHOLD) {
        client.println("<br>");
        return -1;
    }
    client.println(" :Touch detected");
    client.println("<br>");
    return 0;
}

static int printHtmlHeader(WiFiClient client, int update, String ipAddress) {
    client.println("<head><title>Test Page</title>");
    client.println("<meta http-equiv=\"Content-type\" content=\"text/html; charset=UTF-8\">");
    if (update) {
        client.print("<meta http-equiv=\"refresh\" content=\"");
        client.print(update);
        client.print(";URL=http://");
        client.print(ipAddress);
        client.println("/\">");
    }
    client.println("</head>");
    return 0;
}

static int printHtmlBody(WiFiClient client, int update, String ipAddress) {
    uint16_t touchData;

    client.println("<body>");
    client.println("<h3>Wi-Fi & Touch Sensor Test</h3>");

    touchData = touchRead(T6);
    if (printMsgIfTouched(client, touchData, "F7/D6") == 0) {
        Serial.println("T6/D6/ 7pin :" + String(touchData) + " :Touch detected");
    }

    touchData = touchRead(T3);
    if (printMsgIfTouched(client, touchData, "F9/D7") == 0) {
        Serial.println("T3/D7/ 9pin :" + String(touchData) + " :Touch detected");
    }

    touchData = touchRead(T0);
    if (printMsgIfTouched(client, touchData, "F22/D2") == 0) {
        Serial.println("T0/D2/22pin :" + String(touchData) + " :Touch detected");
    }

    touchData = touchRead(T5);
    if (printMsgIfTouched(client, touchData, "F26/D4") == 0) {
        Serial.println("T5/D4/26pin :" + String(touchData) + " :Touch detected");
    }

    touchData = touchRead(T4);
    if (printMsgIfTouched(client, touchData, "F28/D5") == 0) {
        Serial.println("T4/D5/28pin :" + String(touchData) + " :Touch detected");
    }

    client.print("Update interval = ");
    client.print(update);
    client.println(" second</p>");
    client.println("<hr>");
    client.print("<p>http://");
    client.print(ipAddress);
    client.println("<form method=\"POST\" action=\"http://" + ipAddress + "/\">");
    
    client.println("<p>ユーザID：<br>");
    client.println("<input type=\"text\" name=\"userid\"></p>");
    client.println("<p>SSID：<br>");
    client.println("<input type=\"text\" name=\"ssid\"></p>");
    client.println("<p>PASSWORD：<br>");
    client.println("<input type=\"text\" name=\"password\"></p>");
    client.println("<button type=\"submit\">設定</button>");
    // client.println("automatic update:<input type=\"submit\" name=\"INT\" value=\"0 Stop\">");
    // client.println("<input type=\"submit\" name=\"INT\" value=\"1 second\">");

    client.println("</form>");
    client.println("</body>");
    return 0;
}

static String ipUintToString(uint32_t ip) {
    char ipCharArray[16];
    sprintf(ipCharArray,"%d.%d.%d.%d",
        ip & 255,
        ip>>8 & 255,
        ip>>16 & 255,
        ip>>24
    );
    return String(ipCharArray);
}

static void connectHtml(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");              // response HTTP OK
    client.println("Content-Type: text/html");
    client.println("Connection: close");            // close session after sending all data
    client.println();
}


//加速度センサのデータを表示
static void printAccelData(WiFiClient client, int update, String ipAddress, String accelStr) {
    client.print("<p>");
    client.print(accelStr);
    client.print("</p>");
}

static void dispComplete(WiFiClient client) {
    client.print("<p>");
    client.print("設定が完了しました。");
    client.print("</p>");
}

void htmlTouchSensorMain(WiFiClient client, int update, uint32_t ip, String accelStr, bool isSetup) {
    String ipAddress = ipUintToString(ip);

    client.println("<html>");

    printHtmlHeader(client, update, ipAddress);

    printHtmlBody(client, update, ipAddress);

    printAccelData(client, update, ipAddress, accelStr);

    client.println("</html>");

    if (isSetup) {
        dispComplete(client);
    }
}
#endif
