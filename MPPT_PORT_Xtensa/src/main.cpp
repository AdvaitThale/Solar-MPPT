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
#include <ACS712.h>
#include <WiFi.h>
#include <WebServer.h>

// Local SSID
#define WIFI_SSID "MPPT Rev 2.3"
#define WIFI_PASSWORD "Mppt_N3t"

// Network SSID
// #define WIFI_SSID "Issac Newton 🍎"
// #define WIFI_PASSWORD "amhipuneri"

/* Put IP Address details */
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80);

#define CUR 13     // ACS712
#define MOSFET_1 2 // MOSFET Driver Opto-Isolator 2 // Built-In LED Pin
#define MOSFET_2 4 // MOSFET Driver Opto-Isolator 2
#define BUZZ 15    // External Buzzer
#define LM 14      // LM-35
#define IREC 23    // IR Receiver
#define PBJT 18    // Battery Charge Control BJT
#define BVOL 34    // Battery Voltage ADC
#define SVOL 35    // Input Voltage ADC
#define dt 2000

int RAW_TEMP, BAT_VOL, SOL_VOL, PLAY = 0;
float BAT_VOL_R_FACT, SOL_VOL_R_FACT = 0.09;
float PID_TEMP, BAT_CURRENT, BAT_VOLTAGE, SOL_VOLTAGE;

byte TEMP[] = {0x04, 0x0A, 0x0A, 0x0A, 0x0E, 0x1F, 0x1F, 0x0E};    // Thermometer Character
byte CELSIUS[] = {0x18, 0x18, 0x07, 0x08, 0x08, 0x08, 0x08, 0x07}; // Modified °C Character
byte BATTERY[] = {0x0E, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F}; // Battery Character
byte PANEL[] = {0x1F, 0x15, 0x1F, 0x15, 0x1F, 0x15, 0x1F, 0x00};   // PV Panel Character

LiquidCrystal_I2C lcd(0x27, 16, 2);
ACS712 sensor(ACS712_05B, 13);

void greetings();
void SOLAR_INFO();
void BATTERY_INFO();
void handle_OnConnect();
void handle_NotFound();
String SendHTML();

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
  pinMode(BVOL, INPUT);      // ADC Pin for Battery Voltage
  pinMode(SVOL, INPUT);      // ADC Pin for Input Voltage
  pinMode(MOSFET_1, OUTPUT); // MOSFET Driver Pin A
  pinMode(MOSFET_2, INPUT);  // MOSFET Driver Pin B
  pinMode(BUZZ, OUTPUT);     // Buzzer Pin for Beeps
  pinMode(LM, INPUT);        // ADC Pin for LM35 Temp.
  pinMode(IREC, INPUT);      // IR Receiver VS1838B
  lcd.init();                // LCD Initialise
  lcd.backlight();           // LCD Backlight ON
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD); // Set it to NULL, if you don’t want to set a password.
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  Serial.print("Connecting ");
  greetings(); // Device Start Configuration
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
}

void loop()
{
  server.handleClient();
  SOLAR_INFO();
  delay(dt);
  BATTERY_INFO();
  delay(dt);
}

void handle_OnConnect()
{
  loop();
  server.send(200, "text/html", SendHTML(SOL_VOLTAGE, BAT_CURRENT, PID_TEMP));
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Device is Not Connected to ESP32!!");
}

void greetings()
{
  lcd.setCursor(0, 0);
  lcd.print("MPPT Rev 2.3");
  lcd.setCursor(0, 1);
  lcd.print("----------------");
  Serial.println("Initializing MPPT...");
  // digitalWrite(MOSFET_1, HIGH);
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

void SOLAR_INFO()
{
  lcd.createChar(9, TEMP);
  lcd.createChar(10, CELSIUS);
  lcd.createChar(11, PANEL);
  lcd.home();
  SOL_VOL = analogRead(SVOL);
  SOL_VOLTAGE = SOL_VOL * (4.0 / 4096) * 10;
  BAT_CURRENT = (sensor.getCurrentAC() / 1000);
  RAW_TEMP = analogRead(LM);           // Read LM ADC
  PID_TEMP = ((RAW_TEMP * 1.22) / 10); // Convert ADC value to equivalent voltage and divide since LM35 gives output of 10mv/°C*/
  lcd.setCursor(0, 0);
  lcd.print("VOL:");
  lcd.setCursor(4, 0);
  lcd.print(SOL_VOLTAGE);
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
  lcd.setCursor(11, 1);
  lcd.write(11);
  Serial.print(" | SOLAR | ");
  Serial.print("VOLTAGE: ");
  Serial.print(SOL_VOLTAGE);
  Serial.print(" CURRENT: ");
  Serial.print(BAT_CURRENT);
  Serial.print(" BATTERY TEMP.: ");
  Serial.print(PID_TEMP);
  Serial.print(" °C ");
  // Serial.print(SOL_VOL);
}
void BATTERY_INFO()
{
  lcd.createChar(9, TEMP);
  lcd.createChar(10, CELSIUS);
  lcd.createChar(12, BATTERY);
  lcd.home();
  BAT_VOL = analogRead(BVOL);
  BAT_VOLTAGE = BAT_VOL * (4.0 / 4096);
  BAT_CURRENT = (sensor.getCurrentAC() / 1000);
  RAW_TEMP = analogRead(LM);           // Read LM ADC
  PID_TEMP = ((RAW_TEMP * 1.22) / 10); // Convert ADC value to equivalent voltage and divide since LM35 gives output of 10mv/°C*/
  lcd.setCursor(0, 0);
  lcd.print("VOL:");
  lcd.setCursor(4, 0);
  lcd.print(BAT_VOLTAGE);
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
  lcd.setCursor(11, 1);
  lcd.write(12);
  Serial.print(" | BATTERY | ");
  Serial.print("VOLTAGE: ");
  Serial.print(BAT_VOLTAGE);
  Serial.print(" CURRENT: ");
  Serial.print(BAT_CURRENT);
  Serial.print(" BATTERY TEMP.: ");
  Serial.print(PID_TEMP);
  Serial.println(" °C  |");
}
String SendHTML(float SOL_VOLTAGE, float BAT_CURRENT, float PID_TEMP, float BAT_VOLTAGE)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<meta content=\"Advait Thale\" name=\"author\"><meta content=\"1\" http-equiv=\"refresh\">\n";
  ptr += "<title>MPPT Monitor</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "footer {text-align: center;text-decoration: none;position: fixed;left: 0;bottom: 0;width: 100%;height: 3%;padding: 2px;background-color: #00004d;color: #f2f2f2;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>MPPT MASTER DASHBOARD</h1>\n";
  if (PID_TEMP >= 50)
  {
    ptr += "<p>TEMP. OVERLOAD<svg width=\"170\" height=\"15\"><rect x=\"15\" y=\"5\" rx=\"7\" ry=\"7\" width=\"20\" height=\"10\" style=\"fill: #e60000\"/></svg> </p>";
  }
  else if (PID_TEMP > 25 && PID_TEMP < 50)
  {
    ptr += "<p>TEMP. OVERLOAD<svg width=\"170\" height=\"15\"><rect x=\"15\" y=\"5\" rx=\"7\" ry=\"7\" width=\"20\" height=\"10\" style=\"fill: #ff8800\"/></svg> </p>";
  }
  else
  {
    ptr += "<p>TEMP. OVERLOAD<svg width=\"170\" height=\"15\"><rect x=\"15\" y=\"5\" rx=\"7\" ry=\"7\" width=\"20\" height=\"10\" style=\"fill: #009933\"/></svg> </p>";
  }

  ptr += "<p> BATTERY\n </p>";
  ptr += "<p> Voltage: ";
  ptr += (float)BAT_VOLTAGE;
  ptr += "</p>";
  ptr += "<p> Current: ";
  ptr += (float)BAT_CURRENT;
  ptr += "</p>";
  ptr += "<p> BATTERY TEMP.: ";
  ptr += (float)PID_TEMP;
  ptr += "</p>";
  ptr += "<p> SOLAR\n </p>";
  ptr += "<p> Voltage: ";
  ptr += (float)SOL_VOLTAGE;
  ptr += "</p>";
  ptr += "</div>\n";
  ptr += "<footer>MPPT MASTER DASHBOARD -> @Author: Advait Thale</footer>";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
// void IR_MOSFET_Control()
// {
//   if (IrReceiver.decode())
//   {
//     switch (IrReceiver.decodedIRData.command)
//     {
//     case 0x84:
//       digitalWrite(MOSFET_1, !digitalRead(MOSFET_1));
//       Serial.println("<OUTPUT 1 Engaged>");
//       break;

//     case 0xD9:
//       digitalWrite(MOSFET_2, !digitalRead(MOSFET_2));
//       Serial.println("<OUTPUT 2 Engaged>");
//       break;

//     case 0x89:
//       digitalWrite(BUZZ, !digitalRead(BUZZ));
//       Serial.println("ALARM ON");
//       break;

//     default:
//       Serial.println(IrReceiver.decodedIRData.command, HEX);
//       //        Serial.println("Default");
//     }
//     IrReceiver.resume(); // Receive the Next Value
//   }
// }