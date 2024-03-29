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
                                           328P-U PINOUT
  **************************************************************************************************

                              BUZZ |*| D13         D12 |*| C.START
                               3V3 |*|             D11 |*|
                               GND |*| AREF        D10 |*|
                               CUR |*| A0           D9 |*| RELAY
                                   |*| A1           D8 |*| RELAY
                         PT-100(I) |*| A2           D7 |*|
                         PT-100(O) |*| A3           D6 |*|  -
                               SDA |*| A4           D5 |*| CURRENT
                               SCL |*| A5           D4 |*| AMB TEMP / HUM
                                -  |*| A6           D3 |*|
                                -  |*| A7           D2 |*|
                                   |*| 5V          GND |*|
                                   |*| RST         RST |*|  -
                                   |*| GND         RX0 |*|  TX
                                -  |*| VIN         TX0 |*|  RX

  ***************************************************************************************************


  # Standardised System Variables

  BYTA      Alarm                        ON/OFF
  BYTB      Battery                      Percentage
  BYTO      Outdoor Temperature          C(Degree Celcius)
  BYTI      Current (AC/DC) upto 20A      A(Ampere)
  BYTT      Indoor Temperature           C(Degree Celcius)
  BYTL      Load Power                   W(Watts)
  BYTP      Power                        W(Watts)
  BYTS      Generated(Solar) Power       W(Watts)
  BYTH      Humidity                     Percentage

*/

// #include <math.h>   // Math Functions
// #include <ACS712.h> // ACS712 Functions
#include <DHT.h>  // Ambient Temperature/Humidity Functions
#include <Wire.h> // For communication with I2C devices
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>

// #define CUR 2
// #define DHTPIN 5
// #define BUZZ 4
// #define LED 2
#define dt 2000

long DUR;
const byte IR_RECEIVE_PIN = 2; // IR receiver
float BYTA, BYTB, BYTI, BYTO, BYTT, BYTL, BYTP, BYTS, BYTH, THLD1, THLD2;
byte charge[13] = {0x04, 0x0C, 0x1C, 0x1F, 0x1F, 0x07, 0x06, 0x04};
byte error[14] = {0x00, 0x0E, 0x13, 0x17, 0x1D, 0x19, 0x0E, 0x00};
byte restore[15] = {0x00, 0x0D, 0x13, 0x17, 0x10, 0x10, 0x0E, 0x00};
int ILCD = 11; // LCD Pin
int GLED = 12; // LED Pin
int BUZZ = 4;  // BUZZER Pin

// dht DHT;
// ACS712 sensor(ACS712_05B, 2);
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  Serial.begin(115200); // Program Port Serial Baud
  Serial.println("<--- MPPT Rev_8.07 --->");
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  pinMode(ILCD, OUTPUT);
  pinMode(GLED, OUTPUT);
  //  pinMode(CUR, INPUT);          // ADC Pin for ACS712
  pinMode(BUZZ, OUTPUT); // Buzzer Pin for Beeps
                         //  pinMode(DHTPIN, INPUT);       // DHT11 Pin as Input
  //  Serial1.begin(9600);          // Compensated RS-232 Port Baud 9600
  //  attachInterrupt(ISR, RISING, 0); // Attach Interrupt
  //  lcd.init();                   // LCD Initialise
  //  lcd.backlight();              // LCD Backlight ON
  //  initial();                    // Device Start Configuration
  //  Wire.begin();                 // Initiate wire lib. and I2C
  //  Wire.beginTransmission(0x68); // Start transmission to 0x68(MPU)
  //  Wire.write(0x6B);             // Power Management Register (PWR_MGMT_1)
  //  Wire.write(0);                // Wake up IMU
  //  Wire.endTransmission(true);   // End transmission to I2C slave
  digitalWrite(ILCD, LOW);
  digitalWrite(GLED, LOW);
  digitalWrite(BUZZ, LOW);
}

void loop() {

  if (IrReceiver.decode())
  {
    Serial.println(IrReceiver.decodedIRData.command, HEX);

    if (IrReceiver.decodedIRData.command == 0xDC)
    {
      digitalWrite(ILCD, !digitalRead(ILCD));
      delay(100);
    }
    else if (IrReceiver.decodedIRData.command == 0x84)
    {
      digitalWrite(GLED, !digitalRead(GLED));
      delay(100);
    }
    else if (IrReceiver.decodedIRData.command == 0x9C)
    {
      digitalWrite(BUZZ, !digitalRead(BUZZ));
      delay(100);
    }
    IrReceiver.resume(); // receive the next value
  }

  void MAIN_DISPLAY()
  {
    lcd.setCursor(0, 0); //(C, R)
    lcd.print("SOLAR:");
    lcd.setCursor(6, 0);
    lcd.print("89W"); // BYTS
    lcd.setCursor(11, 0);
    lcd.print("LOAD:");
    lcd.setCursor(16, 0);
    lcd.print("22W"); // BYTL
    lcd.setCursor(0, 1);
    lcd.print("BAT:");
    lcd.createChar(13, charge);
    lcd.setCursor(5, 1);
    lcd.write(byte(13));
    lcd.setCursor(6, 1);
    lcd.print("67%"); // BYTB
    lcd.setCursor(10, 1);
    lcd.print("2H 21M");
  }

  void initial()
  {
    lcd.setCursor(6, 0);
    lcd.print("MPPT");
    lcd.setCursor(0, 1);
    lcd.print("----------------");
    Serial.println("Initializing MPPT...");
    digitalWrite(BUZZ, HIGH);
    delay(85);
    digitalWrite(BUZZ, LOW);
    tone(BUZZ, 4093);
    delay(200);
    noTone(BUZZ);
    digitalWrite(BUZZ, HIGH);
    delay(100);
    digitalWrite(BUZZ, LOW);
    delay(dt);
    lcd.clear();
  }

  void alarm()
  {
    Serial.print("ALERT >> ");
    Serial.println("THRESHOLD EXCEEDED !!");
    lcd.noBacklight();
    lcd.setCursor(3, 0);
    lcd.print("THRESHOLD");
    lcd.setCursor(3, 1);
    lcd.print("EXCEEDED!!");
    tone(BUZZ, 3000);
    delay(500);
    lcd.backlight();
    tone(BUZZ, 500);
    delay(500);
    lcd.noBacklight();
    lcd.setCursor(3, 0);
    lcd.print("THRESHOLD");
    lcd.setCursor(3, 1);
    lcd.print("EXCEEDED!!");
    Serial.println("Restart the Device...");
    Serial.println(" ");
  }

  void batalarm()
  {
    Serial.print("ALERT >> ");
    Serial.println("BATTERY CHARGED!!");
    lcd.noBacklight();
    lcd.setCursor(3, 0);
    lcd.print("BATTERY");
    lcd.setCursor(3, 1);
    lcd.print("CHARGED!!");
    delay(500);
    lcd.backlight();
    delay(500);
    lcd.noBacklight();
    lcd.setCursor(3, 0);
    lcd.print("BATTERY");
    lcd.setCursor(3, 1);
    lcd.print("CHARGED!!");
    Serial.println("Unplug the Battery...");
    Serial.println(" ");
  }

  // create instance of 'irrecv'
  // create instance of 'decode_results'
