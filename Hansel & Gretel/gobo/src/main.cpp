#include <Arduino.h>

int LED_pin = 3;
float Desired_Average_Voltage = 3.0;
int Vp = 5;
int output = (Desired_Average_Voltage / Vp) * 255;
int LDR_pin = A0;
int LDR_read = 0;
int threshold = 0;
int calibration_total = 0;
int laser_pin = 2;


int brightStart = 0;
int requiredMillis = 2000; // 2 seconds


bool cylinders_complete = false;

int get_ambient_light_level() {
  //average the readings for first 1sec of operation
  for (int i = 0; i < 100; i++) {
    calibration_total += analogRead(LDR_pin);
    Serial.println(calibration_total);
    delay(10);
  }
  Serial.print("threshold: ");
  Serial.println(calibration_total/100);
  return calibration_total / 100;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_pin, OUTPUT);
  pinMode(LDR_pin, INPUT);
  pinMode(laser_pin, OUTPUT);
  digitalWrite(laser_pin, HIGH); //turn on laser
  analogWrite(LED_pin, 0); //ensure LED off
  threshold = get_ambient_light_level();
  Serial.print("Threshold set to: ");
  Serial.println(threshold);
}



void loop() {
  // put your main code here, to run repeatedly:


  while (cylinders_complete == false){
    LDR_read = analogRead(LDR_pin);
    Serial.println(LDR_read);


    if (LDR_read > threshold * 2) {//currently arbitrary, if light level is double ambient
      if (brightStart == 0) brightStart = millis(); // start timer
      if (millis() - brightStart >= requiredMillis) {
        Serial.println("BRIGHT");
        analogWrite(LED_pin, output);
        digitalWrite(laser_pin, LOW);
        cylinders_complete = true;
      }
    } else {
      // reset timer and ensure LED off / laser on
      brightStart = 0;
      analogWrite(LED_pin, 0);
      digitalWrite(laser_pin, HIGH);
    }
  }



  // delay(1000);
  // analogWrite(LED_pin, output);
}

