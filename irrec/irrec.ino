/*
  Author: Advait Thale



  IR Receiver Pinout:
  -------------
  |OUT|VCC|GND|
  -------------

*/

//#include <IRremote.hpp>
#include <IRremote.h>
//#include "IRremote.hpp"

const byte IR_RECEIVE_PIN = 2; // IR receiver

int ILCD = 11;   
int GLED = 12;     // LED Pin
int BUZZ = 4;      // BUZZER Pin

           // create instance of 'irrecv'
           // create instance of 'decode_results'

void setup() {
  Serial.begin(115200);
  Serial.println("IR Receiver -> ");
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  pinMode(ILCD, OUTPUT);
  pinMode(GLED, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  digitalWrite(ILCD, LOW);
  digitalWrite(GLED, LOW);
  digitalWrite(BUZZ, LOW);
}


void loop() {
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.command, HEX);
   
    if (IrReceiver.decodedIRData.command == 0xDC) {
      digitalWrite(ILCD, !digitalRead(ILCD));
      delay(100);
    }
    else if (IrReceiver.decodedIRData.command == 0x9C) {
      digitalWrite(BUZZ, !digitalRead(BUZZ));
      delay(100);
    }
     else if (IrReceiver.decodedIRData.command == 0x84) {
      digitalWrite(GLED, !digitalRead(GLED));
      delay(100);
    }
    IrReceiver.resume(); // receive the next value
  }
}
