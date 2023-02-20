#include <Arduino.h>
#include <IRremote.hpp>

//#include <IRremote.h>

int recv_pin = 15;

IRrecv irrecv(recv_pin);
decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
//  pinMode(out, OUTPUT);
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume();
  }
  delay(100);
  0x23DCFD01
}
