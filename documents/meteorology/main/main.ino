// ==================== Librerias para los componentes
#include <DHT.h>         // Libreria DHT
#include <ESP8266WiFi.h> // Libreria ESP8266
#include <Wire.h>        // Libreria para I2C

// ==================== Librerias para los display
#include <OLED.h>              // Libreria para el display OLED
// #include <LiquidCrystal_I2C.h> // Libreria para el display LCD

#include <WifiLocation.h>

OLED display(2,1);                   // Creacion del display OLED
// LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dreacion del display LCD

// ==================== Librerias para el manejo de fechas
#include <NTPClient.h>
#include <WiFiUdp.h>

// ==================== Librerias para la creacion de un servidor web
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// ==================== Librerias para coneccion a base datos de google
#include <FirebaseESP8266.h>    // Firebase para ESP8266
#include <addons/TokenHelper.h> // Generacion de token de acceso
#include <addons/RTDBHelper.h>

// ==================== Datos conexion API ThingSpeak
#define THINK_KEY "1VXGOVNMCWHWT3B2"      //  Ingrese su clave Write API de ThingSpeak
#define THINK_SERVER "api.thingspeak.com" // Servidos Thingspeak (Matlab)

//  ==================== Configuracion Firebase (NO MODIFICAR)
#define DB_ID "hidrologia-u-distrital"                                       // Identificador de la base de datos
#define DB_HOST "https://hidrologia-u-distrital-default-rtdb.firebaseio.com" // Direccion de la base datos
#define DB_API_KEY "AIzaSyC0v-XlIcscm_BlAKdyYxHPzp5CQpmxBgw"                 // Clave de acceso a la bas de datos
#define DB_REST_SECRET "tPtDKT4ZF0nanJ9QyJYZBRMsrnnc3P1Cj0CQPGWT"            // Clave de acceso a la bas de datos

#define GOOGLE_API_GEOLOCATION_KEY "AIzaSyAor4_IQ6zbgIQ44djnjKo1EdsFD8CyqfQ" // Clave de acceso a la API de geolocalizacion
#define KEY_GEOLOC "key=AIzaSyAor4_IQ6zbgIQ44djnjKo1EdsFD8CyqfQ"
#define SERVER_GEOLOC "https://www.googleapis.com/geolocation/v1/geolocale"

// ================================================================================= //
//   Para usar la base de datos de google debe solicitar la creacion del usuario a   //
//   efmarroquinb@udistrital.edu.co, enviando su correo, y datos del proyecto        //
// ================================================================================= //

// ==================== Configuracion Firebase (Datos del proyecto) entregado por el administrador del sistema
#define DB_NODE "edwin-marroquin"                              // Nombre del nodo donde se almacena la informacion
#define FIREBASE_EMAIL "efmarroquinb@correo.udistrital.edu.co" // Correo de validacion
#define FIREBASE_PASS "Maria-0227-"                            // Contraseña de validacion

// ==================== Configuracion del sensor DTH
#define DHTPIN D5         // Define el identificador del pin a usar en la placa ESP8266
#define DHTTYPE DHT11     // Define el tipo de sensor conectador a la placa ESP8266
DHT dht(DHTPIN, DHTTYPE); // Instaciacion de la clase para el Sensor DHT

// ==================== Configuracion placa ESP8266
#define ESP8266_LED 2 // Define el led a manupilar en la placa ESP8266

// ==================== Creacion del servidor local
#define SERVER_PORT 80
ESP8266WebServer server(SERVER_PORT);

// ==================== Conexión Wifi-Servidor
WiFiClient client;
WiFiUDP ntpUDP;
HTTPClient http;

WifiLocation location(GOOGLE_API_GEOLOCATION_KEY);

// ==================== Configuracion Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;

// ==================== Inicializacion de variables
String nodePath;

float lat = 0;
float lon = 0;

float t = 0.0;
float h = 0.0;

int d = 10000;
long dataTime = 0;
int utcOffset = 0;
// Define fecha y hora segun uso horario
// Ajustado en segundos * El uso horario
// GMT+5 =  5
// GMT-5 =  5
// GMT-1 = -1
// GMT   =  0

NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffset * 3600);

// ==================== Incluir diseño webserver local
#include "index.h"

void setup()
{

  // lcd.init(); // initialize the lcd
  // delay(1000);
  // lcd.backlight();
  // lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("Select MeteoLab");
  // lcd.setCursor(0, 1);
  // lcd.print("open 192.168.4.1");

  display.begin(); // initialize the display
  display.print("Select MeteoLab",1);
  display.print("open 192.168.4.1",2);
  display.clear();

  pinMode(ESP8266_LED, OUTPUT); // Configuracion led de la placa ESP8266

  WiFi.mode(WIFI_STA); // Inicia la placa ESP8266 en modo
  // STATION       = WIFI_STA
  // ACCESS POINT  = WIFI_AP

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFiManager wm;

  // ============ Se crea una red wifi provisional
  // ============ con el nombre de la red y la contraseña
  if (!wm.autoConnect("MeteoLab", "hidro2021"))
  {
    // lcd.home();
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("Error en la conexion");
    // lcd.setCursor(0, 1);
    // lcd.print("Intente de nuevo");
    // delay(2000);
    // ESP.reset();
    display.clear();
    display.print("Error en la conexion",1);
    display.print("Intente de nuevo",2);
    delay(2000);
    ESP.reset();
  }


  http.begin(client, (String)SERVER_GEOLOC + "?" + (String)KEY_GEOLOC);
  delay(1000); // See if this prevents the problm with connection refused and deep sleep
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("");
  String payload = http.getString();
  Serial.println(httpCode); // Print HTTP return code
  Serial.println(payload);  // Print request response payload
  http.end();

  timeClient.begin();

  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  // ============= Inicializacion del servidor local
  server.onNotFound(handle_NotFound);
  server.on("/data", []()
            { server.send(200, "application/json", dataJson(dataTime, t, h)); });
  server.on("/", handle_OnConnect);

  server.begin();

  // ============= Coneccion a FireBase
  config.api_key = DB_API_KEY;
  config.database_url = DB_HOST;
  config.token_status_callback = tokenStatusCallback;

  auth.user.email = FIREBASE_EMAIL;
  auth.user.password = FIREBASE_PASS;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);

  // ============= Inicio de trabajo del |sensor DHT11
  dht.begin();
}

void loop()
{

  digitalWrite(ESP8266_LED, LOW); // Enciende el led de la placa ESP8266

  // ============= Toma de lecturas del sensor DHT
  h = dht.readHumidity();
  t = dht.readTemperature();

  // lcd.home();
  // lcd.clear();
  // lcd.setCursor(2, 0);
  // lcd.print(WiFi.localIP());
  // lcd.setCursor(0, 1);
  // lcd.print("T " + String(t) + "  H " + String(h));

  display.clear();
  display.print(WiFi.localIP().toString(),1);
  display.print("T " + String(t) + "  H " + String(h),2);

  if (isnan(h) || isnan(t))
  {
    // lcd.home();
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("Error de lectura");
    // lcd.setCursor(0, 1);
    // lcd.print("Revise el sistema");
    display.clear();
    display.print("Error de lectura",1);
    display.print("Revise el sistema",2);
    return;
  }

  if (client.connect(THINK_SERVER, SERVER_PORT))
  {
    String postStr = THINK_KEY;

    // ============= Creacion del nodo con una marca de tiempo para almacenar
    // ============= la nformacion en la base de datos de FireBase
    timeClient.update();
    dataTime = timeClient.getEpochTime();
    nodePath = (String)DB_NODE + "/" + String(dataTime);

    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);

    // ==================== Creacion de cabeceras REST para el envio de la informacion
    // ==================== a la pagina de thinkspeak.com
    Serial.println("// ============= Saving in ThinkSpeak");
    client.print("POST /update HTTP/1.1\n");
    client.print((String) "Host: " + THINK_SERVER + "\n");
    client.print("Connection: close\n");
    client.print((String) "X-THINGSPEAKAPIKEY: " + THINK_KEY + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("// ============= Saved in ThinkSpeak");
  }

  client.stop();
  client.flush();

  Serial.println("\n// ===========================================");
  Serial.println((String) "Sending temperature (°C): " + String(t));
  Serial.println((String) "Sending humidity     (%): " + String(h));

  if (client.connect(DB_HOST, 80))
  {
    Serial.println("\n// ============= Saving in Firebase");
    Serial.println(Firebase.setFloat(fbdo, nodePath + "/temp", t) ? "// ==== Saving Temperature" : fbdo.errorReason().c_str());
    Serial.println(Firebase.setFloat(fbdo, nodePath + "/humi", h) ? "// ==== Saving Humidity" : fbdo.errorReason().c_str());
    Serial.println("// ============= Saved in Firebase\n");
  }

  digitalWrite(ESP8266_LED, HIGH); // Apaga el led de la placa ESP8266

  server.handleClient();

  MDNS.update();

  delay(d); // Tiempo de retraso entre las lecturas
  // Se recomiendan minimo 10000 milisegundos entre lecturas
  // pero para no saturar la base de datos se recomiendan
  // lecturas entre 30000 milisegundos y 120000 milisegundos

  // lcd.clear();
  display.clear();
}

void handle_OnConnect()
{
  String index = index_html;
  server.send(200, "text/html", index_html);
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Opps! regrese a la pagina anterior");
}

String dataJson(int d, float t, float h)
{
  String dataJ = "{ \"d\": ";
  dataJ += String(d);
  dataJ += " , \"t\" : ";
  dataJ += String(t);
  dataJ += " , \"h\" : ";
  dataJ += String(h);
  dataJ += "}";

  return dataJ;
}

// TODO:
//          1. Configuracion de la base de datos desde el dispositivo movil
//          2. Almacenamiento en Firebase de los datos en formato Json
