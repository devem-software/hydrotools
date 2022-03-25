// ==================== Inicializacion de variables
#define SSID "METEOROLOGIA"
#define PASS "estacion2022"

#define SERVER_PORT 80

String nodePath;

float lat = 0.0;
float lon = 0.0;

float t = 0.0;
float h = 0.0;

int d = 10000;
long dataTime = 0;
int utcOffset = -5;
// Define fecha y hora segun uso horario
// Ajustado en segundos * El uso horario
// GMT+5 =  5
// GMT-5 =  5
// GMT-1 = -1
// GMT   =  0

unsigned long sendDataPrevMillis = 0;
