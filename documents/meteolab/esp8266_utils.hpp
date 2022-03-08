#include "config/wifi.h"

void ConnectWiFi() {
  Serial.print("");
  WiFi.mode(WIFI_STA);

  DNSServer dns;

  AsyncWiFiManager wifiManager(&server, &dns);
  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
  wifiManager.autoConnect(hostname);


  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(". ");
  }
}
