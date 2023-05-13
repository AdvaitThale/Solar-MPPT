
#include <Arduino.h>
#include <WiFiMulti.h>

// #define WIFI_SSID "MPPT Rev 2.3"
// #define WIFI_PASSWORD "Mppt_N3t"

#define WIFI_SSID "Issac Newton 🍎"
#define WIFI_PASSWORD "amhipuneri"

#define LED 2

WiFiMulti wifiMulti;

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting ");

  while (wifiMulti.run() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nConnected!!");
}

void loop()
{
  digitalWrite(LED, WiFi.status() == WL_CONNECTED);
}
// #include <Arduino.h>

// #define LED 2
// // #define LED 2

// void setup()
// {
//   pinMode(LED, OUTPUT);
//   // pinMode(LED, INPUT);
//   Serial.begin(115200);
// }

// void loop()
// {
//   digitalWrite(LED, HIGH);
//   Serial.print("LED ON");
//   delay(500);
//   digitalWrite(LED, LOW);
//   Serial.print("LED OFF");
//   delay(500);
// }
