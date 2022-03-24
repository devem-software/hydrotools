#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// ==================== Inicializacion de variables
String nodePath;

float lat = 0.0;
float lon = 0.0;

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
