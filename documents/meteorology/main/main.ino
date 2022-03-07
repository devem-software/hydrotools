// ==================== Librerias para los componentes
#include <DHT.h>             // Libreria DHT
#include <ESP8266WiFi.h>     // Libreria ESP8266


// ==================== Librerias para el manejo de fechas
#include <NTPClient.h>
#include <WiFiUdp.h>


// ==================== Librerias para la creacion de un servidor web
#include <DNSServer.h>
#include <ESP8266WebServer.h >
#include <ESP8266mDNS.h>
#include <WiFiManager.h>


// ==================== Librerias para coneccion a base datos de google
#include <FirebaseESP8266.h>    // Firebase para ESP8266
#include <addons/TokenHelper.h> // Generacion de token de acceso
#include <addons/RTDBHelper.h>


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
ESP8266WebServer server(80);

// ==================== Conexión Wifi-Servidor
WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


// ==================== Configuracion Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;

// ==================== Inicializacion de variables
float t = 0.0;
float h = 0.0;
int d = 10000;
String dataJ = "";

void setup()
{

  pinMode(ESP8266_LED, OUTPUT);   // Configuracion led de la placa ESP8266

  timeClient.begin();
  timeClient.setTimeOffset(-3600 * 5); // Define fecha y hora segun uso horario
  // Ajustado en segundos * El uso horario
  // GMT+1 = 3600*1
  // GMT+8 = 3600*8
  // GMT-1 = 3600*(-1)
  // GMT   = 3600*0

  WiFi.mode(WIFI_STA);                // Inicia la placa ESP8266 en modo
  // STATION       = WIFI_STA
  // ACCESS POINT  = WIFI_AP
  // WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFiManager wm;
  wm.autoConnect("MeteoLab");   // Red WiFi creada por la placa ESP8266
  // Punto en donde se tiene que buscar el acceso
  // para que la lectura inicie

  Serial.println("// ============= WiFi connected\n");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }


  // ============= Inicializacion del servidor local
  server.on("/", handle_OnConnect);
  server.on("/temp", []() { server.send(200, "text/plain", String(t).c_str()); });
  server.on("/humi", []() { server.send(200, "text/plain", String(h).c_str()); });

  dataJ += "{ 't': '";
  dataJ += String(t);
  dataJ += "' , 'h' : '";
  dataJ += String(h);
  dataJ += "'}";

  server.on("/data", []() { server.send(200, "application/json", String(dataJ).c_str()); });

  server.onNotFound(handle_NotFound);
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
  

  digitalWrite(ESP8266_LED, LOW);   // Enciende el led de la placa ESP8266

  // ============= Toma de lecturas del sensor DHT
  h = dht.readHumidity();
  t = dht.readTemperature();
  

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

    Serial.println("\n// ===========================================");
    Serial.println((String)"Sending temperature (°C): " + String(t));
    Serial.println((String)"Sending humidity     (%): " + String(h));

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

  server.handleClient();
  MDNS.update();

  delay(d); // Tiempo de retraso entre las lecturas
  // Se recomiendan minimo 10000 milisegundos entre lecturas
  // pero para no saturar la base de datos se recomiendan
  // lecturas entre 30000 milisegundos y 120000 milisegundos
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(t, h));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Opps! regrese a la pagina anterior");
}

//
String SendHTML(float temp, float humi) {
  String tpl = "<!DOCTYPE html>";
  tpl += "<html lang='es'>";
  tpl += "<head>";
  //  tpl += "<meta http-equiv='refresh' content='5' >";
  tpl += "<meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no'>";
  tpl += "<title>MeteoLab - UD</title>";
  tpl += "<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  tpl += "<link rel='stylesheet' href='https://site-assets.fontawesome.com/releases/v6.0.0/css/all.css'>";
  tpl += "<style>";
  tpl += "*{margin:0;padding:0;box-sizing:border-box;transition:all 0.2s linear}";
  tpl += "html{font-family:'Open Sans', sans-serif, verdana;font-size:16px;height:100vh;overflow:hidden}";
  tpl += "body{display:flex;height:100vh;width:100vw;align-items:center;color:#6a6a6a;flex-direction:column;justify-content:space-evenly}";
  tpl += ".webpage{display:flex;flex-direction:column;justify-content:center;align-items:center;width:calc(100% - 8rem)}";
  tpl += ".webpage--title{margin-bottom:4rem;font-weight:900}";
  tpl += ".data{display:flex;flex-direction:column;align-items:center;width:100%;font-size:1.5rem;margin-bottom:2rem;padding:1.5rem;border-radius:0.5rem;justify-content:center;box-shadow:0 5px 9px -4px}";
  tpl += "@media screen and (min-width: 380px){.data{flex-direction:row} }";
  tpl += ".data--icon{font-size:4rem;flex:1}";
  tpl += ".data--text{flex:2;font-size:1.5rem;margin-left:0.5rem;text-align:center}";
  tpl += "@media screen and (max-width: 600px){.data--text{display:none} }";
  tpl += ".data--value{text-align:right;font-weight:900;display:flex}";
  tpl += ".data--tag{width:2rem;flex:1;display:inline-flex;justify-content:end}";
  tpl += ".temp{color:#f57f17}";
  tpl += ".humi{color:#2979ff}";
  tpl += ".button { background: #2E7D32; color: white; padding: 1rem; border-radius: 0.5rem; border: none; cursor: pointer; }";
  tpl += "</style>";

  tpl += "</head>";
  tpl += "<body>";
  tpl += "<div id='webpage' class='webpage'>";
  tpl += "<h1 class='webpage--title'>MeteoLab - UD</h1>";
  tpl += "<div class='data'>";
  tpl += "<div class='data--icon fa-2x fad fa-temperature-sun temp'></div>";
  tpl += "<div class='data--text'>Temperatura</div>";
  tpl += "<div class='data--value temp'>";
  tpl += "<span id='temp'>";
  tpl += (float)temp;
  tpl += "</span>";
  tpl += "<span class='data--tag'>°C</span>";
  tpl += "</div>";
  tpl += "</div>";
  tpl += "";
  tpl += "<div class='data'>";
  tpl += "<div class='data--icon fa-2x fad fa-hand-holding-water humi'></div>";
  tpl += "<div class='data--text'>Humedad</div>";
  tpl += "<div class='data--value humi'>";
  tpl += "<span id='humi'>";
  tpl += (float)humi;
  tpl += "</span>";
  tpl += "<span class='data--tag'>%</span>";
  tpl += "</div>";
  tpl += "</div>";
  tpl += "<div class='data button' id='refresh'> <div class='data--icon fa-3x fad fa-arrows-rotate'></div> </div>";
  tpl += "</div>";
  tpl += "<div>";
  tpl += "Creado por Edwin Marroquin";
  tpl += "</div>";
  tpl += "</body>";
  tpl += "\n";
  
  tpl += "<script>\n";
  tpl += "setInterval(UpdateDoc('humi'),";
  tpl += (int)d;
  tpl += ");\n";
  tpl += "setInterval(UpdateDoc('temp'),";
  tpl += (int)d;
  tpl += ");\n";
  tpl += "function UpdateDoc(page) {\n";
    tpl += "console.log('Updating ' + page);\n";
    tpl += "var xhttp = new XMLHttpRequest();\n";
    tpl += "xhttp.onreadystatechange = async function () {\n";
      tpl += "if (this.readyState == 4 && this.status == 200) {\n";
        tpl += "document.getElementById(page).innerHTML= await this.responseText;\n";
      tpl += "}\n";
    tpl += "};\n";
    tpl += "xhttp.open('GET', '/' + page, true);\n";
    tpl += "xhttp.send();\n";
    tpl += "console.log('Update ' + page);\n";
  tpl += "}\n";
  tpl += "document.getElementById('refresh').addEventListener('click', function () {\n";
  tpl += "console.log('Refreshing');\n";
  tpl += "UpdateDoc('humi');\n";
  tpl += "UpdateDoc('temp');\n";
  tpl += "})\n";
  tpl += "</script>\n";

  tpl += "</html>";
  return tpl;
}

// TODO:
//          1. Configuracion de la base de datos desde el dispositivo movil
//          2. Almacenamiento en Firebase de los datos en formato Json
//          3. Control de placa y sensores mediante plataforma web
