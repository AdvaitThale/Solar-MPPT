
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128    // OLED Width, in Px
#define SCREEN_HEIGHT 64    // OLED Height, in Px
#define OLED_RESET -1       // QT-PY / XIAO
#define VOLT_READ 1         // Defining analogpin for reading battery voltage
#define ZERO 0

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int16_t A, B;
float x, y, z;


void setup() {
  Serial.begin(115200);
  display.begin(0x3C, true); // Address 0x3C default
//  display.setContrast (5);          // dim display
//  display.cp437(true);
  display.display();
  display.clearDisplay();           // Clear the buffer.
  pinMode(VOLT_READ, INPUT);
}

void loop() {
  if (Serial.available() > 3) { //If byte is used instead of int input must be in 3 digit (001, 255)
    //int x = Serial.parseInt();
    int x = Serial.read();
    Serial.println(x);
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 25);
    display.print("X: ");
    display.setCursor(0, 45);
    display.print(x);
  }

  //  display.setCursor(32, 25);
  //  display.print(r);
  //  display.setCursor(0, 40);
  //  display.print("Pitch: ");
  //  display.setCursor(40, 40);
  //  display.print(p);
  //  display.setCursor(0, 55);
  //  display.print("Yaw: ");
  //  display.setCursor(30, 55);
  //  display.print(y);

}
