#include <Arduino.h>
#include <IRremote.h>

#define BUZZ 15
#define MOSFET_1 4
#define MOSFET_2 2

const byte IR_RECEIVE_PIN = 23;

void setup()
{
  Serial.begin(115200);
  pinMode(MOSFET_1, OUTPUT);
  pinMode(MOSFET_2, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  while (!Serial) // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("Waiting for IR Signal...");
}

void loop()
{
 digitalWrite(MOSFET_1, HIGH);
 Serial.print("MOSFET_1 ON");
 delay(1000);
 digitalWrite(MOSFET_1, LOW);
 digitalWrite(MOSFET_2, HIGH);
 Serial.print("MOSFET_2 ON");
 delay(1000);
 digitalWrite(MOSFET_1, LOW);
 digitalWrite(BUZZ, HIGH);
 Serial.print("BUZZER ON");
 delay(1000);
 digitalWrite(BUZZ, LOW);
}








//  if (IrReceiver.decode())
//   {
//     switch (IrReceiver.decodedIRData.command)
//     {
//     case 0x92:
//       digitalWrite(MOSFET_1, !digitalRead(MOSFET_1));
//       Serial.println("<OUTPUT 1 Engaged>");
//       break;

//     case 0x93:
//       digitalWrite(MOSFET_2, !digitalRead(MOSFET_2));
//       Serial.println("<OUTPUT 2 Engaged>");
//       break;

//     case 0x9C:
//       digitalWrite(BUZZ, !digitalRead(BUZZ));
//       //  Serial.println("btn3");
//       break;

//     default:
//       Serial.println(IrReceiver.decodedIRData.command, HEX);
//       //        Serial.println("Default");
//     }
//     IrReceiver.resume(); // Receive the Next Value
//   }