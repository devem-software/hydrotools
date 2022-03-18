// ==================== Librerias para los componentes
#include <DHT.h>         // Libreria DHT
#include <ESP8266WiFi.h> // Libreria ESP8266

#include <LiquidCrystal_I2C.h>

#include <WifiLocation.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Definicion del LCD

// ==================== Librerias para el manejo de fechas
#include <NTPClient.h>
#include <WiFiUdp.h>

// ==================== Librerias para la creacion de un servidor web
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>

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
#define DB_REST_SECRET "tPtDKT4ZF0nanJ9QyJYZBRMsrnnc3P1Cj0CQPGWT"                // Clave de acceso a la bas de datos

#define GOOGLE_API_GEOLOCATION_KEY "AIzaSyAor4_IQ6zbgIQ44djnjKo1EdsFD8CyqfQ" // Clave de acceso a la API de geolocalizacion

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
#define ESP8266_LED 16 // Define el led a manupilar en la placa ESP8266

// ==================== Creacion del servidor local
ESP8266WebServer server(80);

// ==================== Conexión Wifi-Servidor
WiFiClient client;
WiFiUDP ntpUDP;

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

  lcd.init(); // initialize the lcd
  delay(1000);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select MeteoLab");
  lcd.setCursor(0, 1);
  lcd.print("open 192.168.4.1");

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
    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error en la conexion");
    lcd.setCursor(0, 1);
    lcd.print("Intente de nuevo");
    delay(2000);
    ESP.reset();
  }

  location_t loc = location.getGeoFromWiFi();

  Serial.println("Location request data");
  Serial.println(location.getSurroundingWiFiJson());
  Serial.println("Latitude: " + String(loc.lat, 7));
  Serial.println("Longitude: " + String(loc.lon, 7));
  Serial.println("Accuracy: " + String(loc.accuracy));

  timeClient.begin();

  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266"))
  {
    Serial.println("MDNS responder started");
  }

  // ============= Inicializacion del servidor local
  server.onNotFound(handle_NotFound);
  server.on("/", handle_OnConnect);
  server.on("/data", []()
            { server.send(200, "application/json", dataJson(dataTime, t, h)); });
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

  lcd.home();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print(WiFi.localIP());
  lcd.setCursor(0, 1);
  lcd.print("T " + String(t) + "  H " + String(h));

  if (isnan(h) || isnan(t))
  {
    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error de lectura");
    lcd.setCursor(0, 1);
    lcd.print("Revise el sistema");
    return;
  }

  if (client.connect(THINK_SERVER, 80))
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

  lcd.clear();
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
