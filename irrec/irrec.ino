/*
  Author: Advait Thale


  IR Remote Keypad Hex Map:
  
        +------------------+              
        |              DC  |
        |                  |
        |  92    93    CC  |
        |  8E    8F    C8  |
        |  8A    8B    C4  |
        |  CF    87    82  |
        |                  |
        |  80    9C    85  |
        |  81  8C  DD  86  |
        |                  |
        |        CA        |
        |  88 99 CE C1 C5  |
        |        D2        |
        |                  |
        |  DA          D6  |
        |      98  95      |
        |                  |
        |  CD    91    D0  |
        |  8D  97  C3  83  |
        |                  |
        |  89  84  D9  96  |
        |                  |
        |     hathw@y      |
        +------------------+


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
