#include <Arduino.h>

int LED_pin = 3;
float Desired_Average_Voltage = 3;
int Vp = 5;
int output = (Desired_Average_Voltage / Vp) * 255;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_pin, OUTPUT);

  analogWrite(LED_pin, output);

}



void loop() {
  
}

