#include "selectMainFile.h"
#ifdef SELECT_SERVER
#include <WiFiClient.h>

void htmlTouchSensorMain(WiFiClient client, int update, uint32_t ip, String accelStr, bool isSetup);
#endif