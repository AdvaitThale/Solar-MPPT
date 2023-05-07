/*
  Author: Advait Thale

  Sketched a code to obtain HEX code and control peripherals using
  IR Remote with Microcontroller's Digital Out. Update the Keymap
  as per equivalent remote.


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
  Sheildless:
  -------------
  |OUT|VCC|GND|
  -------------

  VS1838B:
  -------------
  |OUT|GND|VCC|
  -------------
  
*/


#include <IRremote.h>

const byte IR_RECEIVE_PIN = 2; // IR receiver

int ILCD = 11;     // LCD Pin
int GLED = 12;     // LED Pin
int BUZZ = 4;      // BUZZER Pin

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
    switch (IrReceiver.decodedIRData.command) {
      case 0x84:
        digitalWrite(GLED, !digitalRead(GLED));
        //Serial.println("btn1");
        break;

      case 0xD9:
        digitalWrite(ILCD, !digitalRead(ILCD));
        //Serial.println("btn2");
        break;

      case 0x89:
        digitalWrite(BUZZ, !digitalRead(BUZZ));
        //Serial.println("btn3");
        break;

      default:
        Serial.println(IrReceiver.decodedIRData.command, HEX);
//        Serial.println("Default");
    }
    IrReceiver.resume(); // Receive the Next Value
  }
}



/*
    if (irrecv.decode(&results)) {
    switch (results.value) {
      case 16582903:
        lcd.print("btn1"); // Button 1
        delay(600);
        lcd.begin(16, 2);
        break;
      case 16615543:
        lcd.print("btn2"); // Button 2
        delay(600);
        lcd.begin(16, 2);
        break;
      case 16599223:
        lcd.print("btn3"); // Button 3
        delay(600);
        lcd.begin(16, 2);
        break;
      default:
        Serial.println(results.value);
    }
    irrecv.resume(); // Receive the next value
  }
  }
*/
