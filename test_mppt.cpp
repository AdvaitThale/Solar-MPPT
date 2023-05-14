#include <Arduino.h>

#define LED 2

void setup()
{
  pinMode(LED, OUTPUT);
  // pinMode(LED, INPUT);
  Serial.begin(115200);
}

void loop()
{
  digitalWrite(LED, HIGH);
  Serial.print("LED ON");
  delay(500);
  digitalWrite(LED, LOW);
  Serial.print("LED OFF");
  delay(500);
}
