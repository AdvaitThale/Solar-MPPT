#include <Arduino.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

// #define WIFI_SSID "MPPT Rev 2.3"
// #define WIFI_PASSWORD "Mppt_N3t"

#define WIFI_SSID "Issac Newton 🍎"
#define WIFI_PASSWORD "amhipuneri"

#define LED 2

bool isConnected = false;

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting ");
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED && !isConnected)
  {
    Serial.print("\nConnected to ");
    Serial.println(WIFI_SSID);
    digitalWrite(LED, HIGH);
    isConnected = true;
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    digitalWrite(LED, !digitalRead(LED));
    delay(500);
    isConnected = false;
  }
}
