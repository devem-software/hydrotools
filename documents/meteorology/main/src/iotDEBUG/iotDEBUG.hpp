#include "iotDEBUG.h"

void debugInfo(){
  if (!wm.autoConnect(SSID, PASS))
    printError("Error en la conexión\n\nIntente de nuevo");

  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266"))
    Serial.println("MDNS responder started");
}
