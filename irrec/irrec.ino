
//#include <Arduino.h>
//#include <IRremote.hpp>
//#include <IRremote.h>
#include "IRremote.hpp"

#define receiver 15 // IR receiver, Connect the Y pin of the Keyes IR sensor with the GPIO 26 of ESP32 

int GLED = 13; // LED Pin
int BUZZ = 4; // BUZZER Pin

IRrecv irrecv(receiver);           // create instance of 'irrecv'
decode_results results;            // create instance of 'decode_results'

void setup() {
  Serial.begin(115200);
  Serial.println("IR Receiver -> ");
  irrecv.enableIRIn(); // Start the receiver
  pinMode(GLED, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  digitalWrite(GLED, LOW);
  digitalWrite(BUZZ, LOW);
}


void loop() {
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    Serial.println(results.value, HEX);
    //    translateIR();
    if (results.value == 0x23DCFD01) {
      digitalWrite(GLED, !digitalRead(GLED));
      delay(100);
    }

    else if (results.value == 0x639CFD01) {
      digitalWrite(BUZZ, !digitalRead(BUZZ));
      delay(100);
    }

    else {
      Serial.println("Invalid Command!!");
      delay(100);
    }
    irrecv.resume(); // receive the next value
  }
}
