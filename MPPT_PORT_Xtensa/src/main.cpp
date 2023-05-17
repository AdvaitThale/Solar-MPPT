/*
  Author: Advait Thale
  Board: ESP32 Dev Module
  Baud: 115200
  Flash Freq: 80MHz

  *************************************************************************************************
                                         MPPT_PORT-Xtensa
  *************************************************************************************************

  This test code is an overview of how MPPT works on the port of ATMega328P.
  It is equipped with 20*4 Character LCD (for device & energy status), Function
  Buttons, ESP32 Microcontroller, Solar Inlet, 30A FUSE, Sleep Mode, Webserver,
  2 x PT-100(for Indoor & Outdoor Temp.), RS-232 Interface(at 9600),Program
  Serial(at 115200), Internal DIP Switches (for system configuration), Relay
  driven Outputs(for automation purpose), I/O for interfacing external
  sensors and automation purpose.
  The live status is displayed on LCD, the Webserver(at 192.168.1.1) as well as
  the data log can be viewed via both RS-232 (DB9) or USB A Serial ports. Refer
  README.md and attached datasheets for furthur details on MPPT.

  **************************************************************************************************
  *                                        ESP32 PINOUT                                            *
  **************************************************************************************************

                               3V3 |*|                       |*| 5V
                               GND |*|                       |*| GND
                              BUZZ |*| GPIO15         GPIO13 |*| ACS712
                              MD#1 |*| GPIO02         GPIO12 |*| PIDFAN
                              MD#2 |*| GPIO04         GPIO14 |*| LM35
                              RX   |*| GPIO16         GPIO27 |*|  -
                              TX   |*| GPIO17         GPIO26 |*|  -
                               -   |*| GPIO05         GPIO25 |*|  -
                              C.C. |*| GPIO18         GPIO33 |*|  -
                              ESW  |*| GPIO19         GPIO32 |*|  -
                              SDA  |*| GPIO21         GPIO35 |*|  %
                             EDATA |*| GPIO03         GPIO34 |*|  %
                             ECLK  |*| GPIO01         GPIO39 |*| SINE-
                              SCL  |*| GPIO22         GPIO36 |*| SINE+
                             IRREC |*| GPIO23           EN   |*|  -

  ***************************************************************************************************

*/

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
// #include <WiFi.h>

// Local SSID
// #define WIFI_SSID "MPPT Rev 2.3"
// #define WIFI_PASSWORD "Mppt_N3t"

// Network SSID
// #define WIFI_SSID "Issac Newton 🍎"
// #define WIFI_PASSWORD "amhipuneri"

#define CUR 13     // ACS712
#define MOSFET_1 2 // MOSFET Driver Opto-Isolator 2 // Built-In LED Pin
#define MOSFET_2 4 // MOSFET Driver Opto-Isolator 2
#define BUZZ 15    // External Buzzer
#define LM 14      // LM-35
#define IREC 23    // IR Receiver
#define PBJT 18    // Battery Charge Control BJT
#define dt 2000

int RAW_TEMP, PLAY = 0;
float PID_TEMP, BAT_CURRENT;

byte TEMP[] = {0x04, 0x0A, 0x0A, 0x0A, 0x0E, 0x1F, 0x1F, 0x0E};    // Thermometer Character
byte CELSIUS[] = {0x18, 0x18, 0x07, 0x08, 0x08, 0x08, 0x08, 0x07}; // Modified °C Character

LiquidCrystal_I2C lcd(0x27, 16, 2);
// ACS712 sensor(ACS712_05B, 2);

void greetings();
void scrollThrough();

void tone(byte PIN, int FREQ)
{
  ledcSetup(0, 2000, 8);
  ledcAttachPin(PIN, 0);
  ledcWriteTone(0, FREQ);
  PLAY = PIN;
}

void noTone()
{
  tone(PLAY, 0);
}

void setup()
{
  Serial.begin(115200);
  pinMode(CUR, INPUT);       // ADC Pin for ACS712
  pinMode(MOSFET_1, OUTPUT); // MOSFET Driver Pin A
  pinMode(MOSFET_2, INPUT);  // MOSFET Driver Pin B
  pinMode(BUZZ, OUTPUT);     // Buzzer Pin for Beeps
  pinMode(LM, INPUT);        // ADC Pin for LM35 Temp.
  pinMode(IREC, INPUT);      // IR Receiver VS1838B
  lcd.init();                // LCD Initialise
  lcd.backlight();           // LCD Backlight ON
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting ");
  greetings(); // Device Start Configuration
}

void loop()
{
  lcd.createChar(9, TEMP);
  lcd.createChar(10, CELSIUS);
  lcd.home();
  RAW_TEMP = analogRead(LM);           // Read LM ADC
  PID_TEMP = ((RAW_TEMP * 1.22) / 10); // Convert ADC value to equivalent voltage and divide since LM35 gives output of 10mv/°C*/
  Serial.print("BATTERY TEMP.: ");
  Serial.print(PID_TEMP);
  Serial.println(" °C");
  lcd.setCursor(0, 0);
  lcd.print("VOL:");
  lcd.setCursor(8, 0);
  lcd.print("V");
  lcd.setCursor(10, 0);
  lcd.write(9);
  lcd.setCursor(11, 0);
  lcd.print(PID_TEMP);
  lcd.setCursor(15, 0);
  lcd.write(10);
  lcd.setCursor(0, 1);
  lcd.print("CUR: ");
  lcd.setCursor(4, 1);
  lcd.print(BAT_CURRENT);
  lcd.setCursor(8, 1);
  lcd.print("A");
}

void greetings()
{
  lcd.setCursor(0, 0);
  lcd.print("MPPT Rev 2.3");
  lcd.setCursor(0, 1);
  lcd.print("----------------");
  Serial.println("Initializing MPPT...");
  digitalWrite(MOSFET_1, HIGH);
  digitalWrite(BUZZ, !digitalRead(BUZZ));
  delay(85);
  tone(BUZZ, 4093);
  delay(200);
  noTone();
  digitalWrite(BUZZ, !digitalRead(BUZZ));
  delay(100);
  delay(dt);
  lcd.clear();
}

void IR_MOSFET_Control()
{
  if (IrReceiver.decode())
  {
    switch (IrReceiver.decodedIRData.command)
    {
    case 0x84:
      digitalWrite(MOSFET_1, !digitalRead(MOSFET_1));
      Serial.println("<OUTPUT 1 Engaged>");
      break;

    case 0xD9:
      digitalWrite(MOSFET_2, !digitalRead(MOSFET_2));
      Serial.println("<OUTPUT 2 Engaged>");
      break;

    case 0x89:
      digitalWrite(BUZZ, !digitalRead(BUZZ));
      // Serial.println("btn3");
      break;

    default:
      Serial.println(IrReceiver.decodedIRData.command, HEX);
      //        Serial.println("Default");
    }
    IrReceiver.resume(); // Receive the Next Value
  }
}