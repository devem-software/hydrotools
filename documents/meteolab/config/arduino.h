// ==================== Configuracion del sensor DTH

// Define el identificador del pin a usar en la placa ESP8266
#define DHTPIN D5

// Define el tipo de sensor conectador a la placa ESP8266
#define DHTTYPE DHT11

// Instaciacion de la clase para el Sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// ==================== Configuracion placa ESP8266
#define ESP8266_LED 16 // Define el led a manupilar en la placa ESP8266
