#include "config/wifi.h"

void ConnectWiFi() {

  WiFi.mode(WIFI_STA);

  DNSServer dns;

  AsyncWiFiManager wm(&server, &dns);
  wm.setSTAStaticIPConfig(_ip, _gw, _sn);
  wm.autoConnect(hostname, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(". ");
  }
}
