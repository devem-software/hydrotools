#include "iotOLED.h"
#include "inteud.c"

void InitializeOLED()
{
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.cp437(true);
  display.clearDisplay();
  display.drawBitmap(4, 2, image_data_inteud, 120, 62, SSD1306_WHITE);
  display.display();
  delay(3000);

  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void printWiFiManager()
{
  display.setCursor(0, 0);
  display.println((String)"WIFI: " + SSID);
  display.println((String)"PASS: " + PASS);
  display.println();
  display.println("Open in browser:");
  display.println("192.168.4.1");
  display.println("Connect local wifi");
  display.println("Enjoy!");
  display.display();
}

void printError(String error)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("********************");
  display.println(error);
  display.println("********************");
  display.display();
  delay(2000);
}

void printInfo(float oT, float oH)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println();
  display.println("OPEN: " + WiFi.localIP().toString());
  display.println();
  display.setTextSize(2);
  display.print("T " + String(oT) + " ");
  display.write(7);
  display.write(67);
  display.println();
  display.setTextSize(1);
  display.println();
  display.setTextSize(2);
  display.print("H " + String(oH) + " ");
  display.write(37);
  display.display();
}
