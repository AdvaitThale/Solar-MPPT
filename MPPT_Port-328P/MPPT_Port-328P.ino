/*
  Author: Advait Thale

  *************************************************************************************************
                                     MPPT_PORT-328P
  *************************************************************************************************

  This test code is an overview of how MPPT works on the port of ATMega328P.
  It is equipped with 20*4 Character LCD (for device & energy status), Function
  Buttons, ATMega328P Microcontroller, Solar Inlet, 30A FUSE, Sleep Mode,
  2 x PT-100(for Indoor & Outdoor Temp.), RS-232 Interface(at 9600),Program
  Serial(at 115200), Internal DIP Switches (for system configuration), Relay
  driven Outputs(for automation purpose), I/O for interfacing external
  sensors and automation purpose.
  The live status is displayed on LCD as well as the data log can be viewed via
  both RS-232 (DB9) or USB A Serial ports. Refer README.md and attached datasheets
  for furthur details on MPPT.

  **************************************************************************************************
                                           ESP32 PINOUT
  **************************************************************************************************

                               3V3 |*|                       |*| 5V
                               GND |*|                       |*| GND
                               IR  |*| GPIO15         GPIO13 |*| C.START
                               CUR |*| GPIO02         GPIO12 |*| C.STOP
                    AMB TEMP / HUM |*| GPIO04         GPIO14 |*| ALARM
                            L.TRIG |*| GPIO16         GPIO27 |*| P.COUNT
                            L.ECHO |*| GPIO17         GPIO26 |*|  -
                              BUZZ |*| GPIO05         GPIO25 |*| RELAY
                                -  |*| GPIO18         GPIO33 |*| RELAY
                                -  |*| GPIO19         GPIO32 |*| RELAY
                               SDA |*| GPIO21         GPIO35 |*| RELAY
                               RX  |*| GPIO03         GPIO34 |*| PT-100
                               TX  |*| GPIO01         GPIO39 |*| FLOW
                               SCL |*| GPIO22         GPIO36 |*| PRES
                                -  |*| GPIO23           EN   |*|  -

  ***************************************************************************************************


  # Standardised System Variables

  BYTCT     Cycle Start/Cycle Stop       ON/OFF
  BYTA      Alarm                        ON/OFF
  BYTN      Count                        N(Discrete Number)
  BYTO      Outdoor Temperature          C(Degree Celcius)
  BYTI      Current (AC/DC) upto 5A      A(Ampere)
  BYTT      Indoor Temperature           C(Degree Celcius)
  BYTL      Load Power                   W(Watts)
  BYTP      Power                        W(Watts)
  BYTS      Generated(Solar) Power       W(Watts)
  BYTH      Humidity                     Percentage

*/

#include <math.h>   // Math Functions
#include <ACS712.h> // ACS712 Functions
#include <dht.h>    // Ambient Temperature/Humidity Functions
#include <Wire.h>   // For communication with I2C devices
#include <LiquidCrystal_I2C.h>

#define CUR 2
#define TRIG 16
#define ECHO 17
#define DHTPIN 4
#define BUZZ 11
#define dt 2000

long DUR;
float BYTA, BYTT, BYTO, BYTL, BYTP, BYTS, BYTR, BYTI, BYTN, BYTH, BYTX, THLD1, THLD2;
byte charge[13] = {0x04, 0x0C, 0x1C, 0x1F, 0x1F, 0x07, 0x06, 0x04};

dht DHT;
ACS712 sensor(ACS712_05B, 2);
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  pinMode(CUR, INPUT);          // ADC Pin for ACS712
  pinMode(TRIG, OUTPUT);        // TRIGGER Pin of HC-SR05 to Write
  pinMode(ECHO, INPUT);         // ECHO Pin of HC-SR05 to Read
  pinMode(BUZZ, OUTPUT);        // Buzzer Pin for Beeps
  pinMode(DHTPIN, INPUT);       // DHT11 Pin as Input
  Serial.begin(9600);           // Compensated Baud to 9600 for RS-232
  lcd.init();                   // LCD Initialise
  lcd.backlight();              // LCD Backlight ON
  initial();                    // Device Start Configuration
  Wire.begin();                 // Initiate wire lib. and I2C
  Wire.beginTransmission(0x68); // Start transmission to 0x68(MPU)
  Wire.write(0x6B);             // Power Management Register (PWR_MGMT_1)
  Wire.write(0);                // Wake up IMU
  Wire.endTransmission(true);   // End transmission to I2C slave
}

void loop()
{
  lcd.setCursor(0, 0); //(C, R)
  lcd.print("SOLAR:");
  lcd.setCursor(6, 0);
  lcd.print("89W");
  lcd.setCursor(11, 0);
  lcd.print("LOAD:");
  lcd.setCursor(16, 0);
  lcd.print("22W");
  lcd.setCursor(0, 1);
  lcd.print("BAT:");
  lcd.createChar(13, charge);
  lcd.setCursor(5, 1);
  lcd.write(byte(13));
  lcd.setCursor(6, 1);
  lcd.print("67%");
  lcd.setCursor(9, 1);
  lcd.print("2H 21M");
}

void initial()
{
  lcd.setCursor(3, 0);
  lcd.print("MPPT");
  lcd.setCursor(0, 1);
  lcd.print("----------------");
  Serial.println("Initializing MPPT...");
  digitalWrite(BUZZ, HIGH);
  delay(85);
  digitalWrite(BUZZ, LOW);
  tone(BUZZ, 4093);
  delay(200);
  noTone();
  digitalWrite(BUZZ, HIGH);
  delay(100);
  digitalWrite(BUZZ, LOW);
  delay(dt);
  lcd.clear();
}
