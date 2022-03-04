// ==================== Librerias para los componentes
#include <DHT.h>             // Libreria DHT
#include <ESP8266WiFi.h>     // Libreria ESP8266


// ==================== Librerias para el manejo de fechas
#include <NTPClient.h>
#include <WiFiUdp.h>


// ==================== Librerias para la creacion de un servidor web
#include <DNSServer.h>
#include <ESP8266WebServer.h >
#include <WiFiManager.h>


// ==================== Librerias para coneccion a base datos de google
#include <FirebaseESP8266.h>    // Firebase para ESP8266
#include <addons/TokenHelper.h> // Generacion de token de acceso
#include <addons/RTDBHelper.h>


// ==================== L19 - CIUDAD BOLIVAR
// const char* ssid =  "ALL-BLACKS-NZ"; // Ingrese el nombre de la red WiFi
// const char* password =  "80250114";  // Ingrese la clave de la red WiFi

// ==================== L4 - SANTA FE
// #define WIFI_SSID  "DANIEL P"   // Ingrese el nombre de la red WiFi
// #define WIFI_PASS  "3214497681" // Ingrese la clave de la red WiFi


// ==================== Datos conexion API ThingSpeak
#define THINK_KEY "1VXGOVNMCWHWT3B2"       //  Ingrese su clave Write API de ThingSpeak
#define THINK_SERVER "api.thingspeak.com"  // Servidos Thingspeak (Matlab)


//  ==================== Configuracion Firebase (NO MODIFICAR)
#define DB_ID "hidrologia-u-distrital"                                       // Identificador de la base de datos
#define DB_HOST "https://hidrologia-u-distrital-default-rtdb.firebaseio.com" // Direccion de la base datos
#define DB_API_KEY "AIzaSyC0v-XlIcscm_BlAKdyYxHPzp5CQpmxBgw"                 // Clave de acceso a la bas de datos


// ================================================================================= //
//   Para usar la base de datos de google debe solicitar la creacion del usuario a   //
//   efmarroquinb@udistrital.edu.co, enviando su correo, y datos del proyecto        // 
// ================================================================================= //


// ==================== Configuracion Firebase (Datos del proyecto) entregado por el administrador del sistema
#define DB_NODE "edwin-marroquin"                                            // Nombre del nodo donde se almacena la informacion
#define FIREBASE_EMAIL "efmarroquinb@correo.udistrital.edu.co"               // Correo de validacion
#define FIREBASE_PASS "Maria-0227-"                                          // Contraseña de validacion


// ==================== Configuracion del sensor DTH
#define DHTPIN D3           // Define el identificador del pin a usar en la placa ESP8266
#define DHTTYPE DHT11       // Define el tipo de sensor conectador a la placa ESP8266
DHT dht(DHTPIN, DHTTYPE);   // Instaciacion de la clase para el Sensor DHT

// ==================== Configuracion placa ESP8266
#define ESP8266_LED 16  // Define el led a manupilar en la placa ESP8266


// ==================== Conexión Wifi-Servidor
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


// ==================== Configuracion Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;


void setup()
{


  pinMode(ESP8266_LED, OUTPUT);   // Configuracion led de la placa ESP8266

  timeClient.begin();
  timeClient.setTimeOffset(-3600*5);  // Define fecha y hora segun uso horario
                                      // Ajustado en segundos * El uso horario
                                      // GMT+1 = 3600*1
                                      // GMT+8 = 3600*8
                                      // GMT-1 = 3600*(-1)
                                      // GMT   = 3600*0

  WiFi.mode(WIFI_STA);                // Inicia la placa ESP8266 en modo STATION
  // WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  Serial.begin(9600);
  Serial.setDebugOutput(true);

  WiFiManager wm;
  wm.autoConnect("MeteoLab");   // Red WiFi creada por la placa ESP8266
                                // Punto en donde se tiene que buscar el acceso
                                // para que la lectura inicie
  
  Serial.println("// ============= WiFi connected\n");

  // ============= Coneccion a FireBase 
  config.api_key = DB_API_KEY;
  config.database_url = DB_HOST;
  config.token_status_callback = tokenStatusCallback;

  auth.user.email = FIREBASE_EMAIL;
  auth.user.password = FIREBASE_PASS;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);

  // ============= Inicio de trabajo del sensor DHT11 
  dht.begin();
}

void loop()
{

  digitalWrite(ESP8266_LED, LOW);   // Enciende el led de la placa ESP8266
  
  // ============= Toma de lecturas del sensor DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("// ============= Failed to read from DHT sensor!");
    return;
  }
  if (client.connect(THINK_SERVER, 80))
  {
    String postStr = THINK_KEY;

    // ============= Creacion del nodo con una marca de tiempo para almacenar 
    // ============= la nformacion en la base de datos de FireBase
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    String nodePath = (String)DB_NODE + "/" + String(epochTime);

    delay(10);
    Serial.println("\n// ===========================================");
    Serial.println((String)"Sending temperature (°C): " + String(t));
    Serial.println((String)"Sending humidity     (%): " + String(h));

    delay(50);

    if (Firebase.ready() && millis() - sendDataPrevMillis > 1500 || sendDataPrevMillis == 0) {
      Serial.println("\n// ============= Saving in Firebase");
      Serial.println(Firebase.pushFloat(fbdo, nodePath, t) ? "// ==== Saving Temperature" : fbdo.errorReason().c_str());
      Serial.println(Firebase.pushFloat(fbdo, nodePath, h) ? "// ==== Saving Humidity" : fbdo.errorReason().c_str());
      Serial.println("// ============= Saved in Firebase\n");
    }

    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";

    delay(50);

    // ==================== Creacion de cabeceras REST para el envio de la informacion
    // ==================== a la pagina de thinkspeak.com
    Serial.println("// ============= Saving in ThinkSpeak");
    client.print("POST /update HTTP/1.1\n");
    client.print((String)"Host: " + THINK_SERVER + "\n");
    client.print("Connection: close\n");
    client.print((String)"X-THINGSPEAKAPIKEY: " + THINK_KEY + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("// ============= Saved in ThinkSpeak");
  }

  client.stop();
  digitalWrite(ESP8266_LED, HIGH);  // Apaga el led de la placa ESP8266
  
  delay(30000); // Tiempo de retraso entre las lecturas
                // Se recomiendan minimo 10000 milisegundos entre lecturas
                // pero para no saturar la base de datos se recomiendan 
                // lecturas entre 30000 milisegundos y 120000 milisegundos
}

// TODO:
//          1. Configuracion de la base de datos desde el dispositivo movil
//          2. Almacenamiento en Firebase de los datos en formato Json
//          3. Control de placa y sensores mediante plataforma web
