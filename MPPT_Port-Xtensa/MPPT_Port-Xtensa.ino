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
                              SDA  |*| GPIO21         GPIO35 |*|  -
                             EDATA |*| GPIO03         GPIO34 |*|  -
                             ECLK  |*| GPIO01         GPIO39 |*| SINE-
                              SCL  |*| GPIO22         GPIO36 |*| SINE+
                             IRREC |*| GPIO23           EN   |*|  - 
                                
  ***************************************************************************************************
  

  # Standardised System Variables
  
  BYTCT     Cycle Start/Cycle Stop       ON/OFF
  BYTA      Alarm                        ON/OFF
  BYTN      Count                        N(Discrete Number)
  BYTO      Outdoor Temperature          C(Degree Celcius)
  BYTI      Current (AC/DC) upto 5A      A(Ampere)
  BYTB      Battery Temperature           C(Degree Celcius)
  BYTL      Coolant Level                CM(Centimetre)
  BYTP      Ambient Pressure             Bar (= 100,000kPA)
  BYTH      Humidity                     Percentage
  BYTX      Vibration *(if any)          mm(millimetres)
  
*/


#include <math.h>               // Math Functions
//#include <ACS712.h>             // ACS712 Functions                        
#include <Wire.h>               // For communication with I2C devices
#include <LiquidCrystal_I2C.h>

#define CUR 13
#define MDA 2
#define MDB 4
#define BUZZ 15
#define LM 14
#define TRIG 16
#define ECHO 17
#define dt  2000

int play = 0;
float PID_TEMP;
float BYTA, BYTAT, BYTT, BYTL, BYTP, BYTF, BYTR, BYTI, BYTN, BYTH, BYTX, BYTY, BYTZ, THLD1, THLD2;
long DUR;

//dht DHT;
//ACS712 sensor(ACS712_05B, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(CUR, INPUT);                    // ADC Pin for ACS712
  pinMode(MDA, OUTPUT);                   // MOSFET Driver Pin A
  pinMode(MDB, INPUT);                    // MOSFET Driver Pin B
  pinMode(BUZZ, OUTPUT);                  // Buzzer Pin for Beeps
  pinMode(LM, INPUT);                     // ADC Pin for LM35 Temp.
  Serial.begin(115200);                   // Compensated Baud to 9600 for RS-232
  lcd.init();                             // LCD Initialise
  lcd.backlight();                        // LCD Backlight ON
  initial();                              // Device Start Configuration
  Wire.begin();                           // Initiate wire lib. and I2C
  Wire.beginTransmission(0x68);           // Start transmission to 0x68(MPU)
  Wire.write(0x6B);                       // Power Management Register (PWR_MGMT_1)
  Wire.write(0);                          // Wake up IMU
  Wire.endTransmission(true);             // End transmission to I2C slave
}


void loop() {
  PID_TEMP = analogRead(LM);
  Serial.print("BATTERY TEMP.: ");
  Serial.println(PID_TEMP);
  lcd.setCursor(0, 0);
  lcd.print("VOL:");
  lcd.setCursor(9, 0);
  lcd.print("TEMP:");
  lcd.setCursor(14, 0);
  lcd.print(PID_TEMP);
  
}

void tone(byte pin, int freq){
  ledcSetup(0, 2000, 8);
  ledcAttachPin(pin, 0);
  ledcWriteTone(0, freq);
  play = pin;
}

void noTone(){
  tone(play,0);
}

void initial()
{
  lcd.setCursor(0, 0);
  lcd.print("MPPT Rev 2.3");
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
