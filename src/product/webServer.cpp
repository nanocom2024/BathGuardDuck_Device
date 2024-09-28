#include "selectMainFile.h"
#ifdef SELECT_PRODUCT
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
    client.println("<h3>環境設定</h3>");

    //POST用FORM
    client.println("<form method=\"POST\" action=\"http://" + ipAddress + "/\">");
    client.println("<p>Email：<br>");
    client.println("<input type=\"text\" name=\"email\"></p>");
    client.println("<p>SSID：<br>");
    client.println("<input type=\"text\" name=\"ssid\"></p>");
    client.println("<p>PASSWORD：<br>");
    client.println("<input type=\"text\" name=\"password\"></p>");
    client.println("<button type=\"submit\">設定</button>");
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

static void dispComplete(WiFiClient client) {
    client.print("<p>");
    client.print("設定が完了しました。");
    client.print("</p>");
}

void htmlTouchSensorMain(WiFiClient client, int update, uint32_t ip, bool isSetup) {
    String ipAddress = ipUintToString(ip);

    client.println("<html>");

    printHtmlHeader(client, update, ipAddress);

    printHtmlBody(client, update, ipAddress);

    client.println("</html>");

    if (isSetup) {
        dispComplete(client);
    }
}
#endif
