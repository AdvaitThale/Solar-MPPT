#include <Arduino.h>
#include <WiFiMulti.h>


#define WIFI_SSID "MPPT Rev 2.3"
#define WIFI_PASSWORD "Mppt_N3t"

WiFiMulti wifiMulti;

void setup()
{
  Serial.begin(115200);
//  pinMode(LED_BUILTIN, OUTPUT);

  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");

  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Connected!!");
}

void loop()
{
//  digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);
}
