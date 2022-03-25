

// ==================== Librerias para los componentes
#include <DHT.h>         // Libreria DHT
#include <ESP8266WiFi.h> // Libreria ESP8266

#include "src/variables.h"
#include "src/iotDATABASE/iotDATABASE.hpp"
#include "src/iotOLED/iotOLED.hpp"
#include "src/iotGEOLOC/iotGEOLOC.hpp"
#include "src/iotDEBUG/iotDEBUG.hpp"
#include "src/iotWEBSERVER/iotWEBSERVER.hpp"

// ==================== Librerias para el manejo de fechas

// ==================== Librerias para la creacion de un servidor web
// #include <DNSServer.h>
// #include <ESP8266mDNS.h>
#include <WiFiManager.h>
// #include <ESP8266WebServer.h>
// #include <ESP8266HTTPClient.h>

// ==================== Configuracion del sensor DTH
#define DHTPIN D5         // Define el identificador del pin a usar en la placa ESP8266
#define DHTTYPE DHT11     // Define el tipo de sensor conectador a la placa ESP8266
DHT dht(DHTPIN, DHTTYPE); // Instaciacion de la clase para el Sensor DHT

// ==================== Configuracion placa ESP8266
#define ESP8266_LED 2 // Define el led a manupilar en la placa ESP8266

// ==================== Creacion del servidor local
// ESP8266WebServer server(SERVER_PORT);

// ==================== Conexión Wifi-Servidor
WiFiClient client;
// HTTPClient http;

// ==================== Incluir diseño webserver local
// #include "views/index.h"
// #include "views/inteud.h"

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  pinMode(ESP8266_LED, OUTPUT); // Configuracion led de la placa ESP8266

  InitializeOLED();
  printWiFiManager();

  WiFi.mode(WIFI_STA); // Inicia la placa ESP8266 en modo
  // STATION       = WIFI_STA
  // ACCESS POINT  = WIFI_AP

  WiFiManager wm;

  debugInfo();
  // if (!wm.autoConnect(SSID, PASS))
  //   printError("Error en la conexión\n\nIntente de nuevo");

  // Serial.println("IP: ");
  // Serial.println(WiFi.localIP());

  // if (MDNS.begin("esp8266"))
  //   Serial.println("MDNS responder started");

  // ============= Inicializacion del servidor local
  // server.on("/", handle_OnConnect);
  // server.on("/data", handle_ApiData);
  // server.onNotFound(handle_NotFound);
  // server.begin();
  InitializeWebServer();
  InitializeThingSpeak();
  InitializeFirebase();
  InitializeGEOLOC();

  // ============= Inicio de trabajo del |sensor DHT11
  dht.begin();
}

void loop()
{

  digitalWrite(ESP8266_LED, LOW); // Enciende el led de la placa ESP8266

  // ============= Toma de lecturas del sensor DHT
  h = dht.readHumidity();
  t = dht.readTemperature();

  // server.handleClient();
  updateWebServer();
  printInfo(t, h);

  if (isnan(h) || isnan(t))
  {
    printError("Error de lectura\n\nRevise el sistema");
    return;
  }

  Serial.println("\n// ===========================================");
  Serial.println((String) "Sending temperature (°C): " + String(t));
  Serial.println((String) "Sending humidity     (%): " + String(h));

  // if (client.connect(THINK_SERVER, SERVER_PORT))
  SaveThingSpeak(t, h);

  // if (client.connect(DB_HOST, SERVER_PORT))
  SaveFirebase(t, h);

  getGEOLOC();

  digitalWrite(ESP8266_LED, HIGH); // Apaga el led de la placa ESP8266

  MDNS.update();
  delay(d); // Tiempo de retraso entre las lecturas
  // Se recomiendan minimo 10000 milisegundos entre lecturas
  // pero para no saturar la base de datos se recomiendan
  // lecturas entre 30000 milisegundos y 120000 milisegundos
}

// void handle_OnConnect()
// {
//   String index = index_html;
//   server.send(200, "text/html", index_html);
// }

// void handle_ApiData()
// {
//   String data = dataJson(dataTime, t, h);
//   server.send(200, "application/json", data);
// }

// void handle_NotFound()
// {
//   server.send(404, "text/plain", "Opps! regrese a la pagina anterior");
// }

// String dataJson(int d, float t, float h)
// {
//   String dataJ = "{ \"d\": ";
//   dataJ += String(d);
//   dataJ += " , \"t\" : ";
//   dataJ += String(t);
//   dataJ += " , \"h\" : ";
//   dataJ += String(h);
//   dataJ += "}";

//   return dataJ;
// }

// TODO:
//          1. Configuracion de la base de datos desde el dispositivo movil
