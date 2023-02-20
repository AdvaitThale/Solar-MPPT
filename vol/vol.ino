
#define VOL 34

int adc = 0;

void setup(){
  pinMode(VOL, INPUT);
  Serial.begin(115200);
}

 void loop(){
  adc = analogRead(VOL);
  float voltage = 3.3 * (adc/4096.0); 
  Serial.println(voltage);
 }
