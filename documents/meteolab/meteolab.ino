#include <ESP8266WiFi.h>
#include <ESPAsyncWiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include "server.hpp"
#include "esp8266_utils.hpp"

void setup(void) {
  Serial.begin(115200);
  SPIFFS.begin();

  ConnectWiFi();
  InitServer();
}

void loop(void) {
}
