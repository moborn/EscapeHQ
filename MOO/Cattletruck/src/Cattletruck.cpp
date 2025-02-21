/*
 *   rDate: July 2020
 *  Author: Matt Cassidy 
 * 
 *  Title:  Cattletruck Ball Puzzle v1.1
 *  Board:  Arduino Uno r3
 *  Brief:  Single laser counter designed for use in the EscapeHQ puzzle rooms.
 *          Signal pin (maglock) is LOW until puzzle is solved, then HIGH
 *  
 *  Notes:  Puzzle is complete when ball_counter reaches the max_number_balls
 * 
 * Copyright 2020, mattc.creativesolutions@gmail.com
 */

#include <Arduino.h>

#define COM_PIN A0    // <-- Houdini Communication (reserved)
#define SIG_PIN 12    // <-- Signal pin ("cabinet" maglock)
#define LDR_PIN A1    // <-- Light Detecting Resistor (LDR)
#define LSR_PIN 8     // <-- Laser emitter
#define PROX_PIN 13   // <-- E18-D8ONK Proximity Sensor
#define L_DELAY 50    // <-- Loop iteration delay (ms)

const static byte max_number_balls = 14;  // (= 6 + 6 + 2)
bool prox_triggered = false;
bool ball_detected = false;
byte ball_counter = 0;

/* LDR sensor structure */
struct LDR{
  const static byte tolerance = 7;
  uint16_t value, threshold;
  void update(){ value = analogRead(LDR_PIN); }
  bool isHigh(){ 
    this->update();
    //Serial.println(value); // <-- DEBUG!
    return value > threshold + tolerance; 
  }
  void calibrate(){
    this->update();
    threshold = value;
    for(byte i = 0; i < 50; i++){
      this->update();
      if(value < threshold) threshold = value;
    }
  }
} sensor;

/* Description: Startup */
void setup() {
  Serial.begin(9600);
  pinMode(LSR_PIN, OUTPUT);
  pinMode(SIG_PIN, OUTPUT);
  pinMode(PROX_PIN, INPUT_PULLUP);
  prox_triggered = false;
  ball_detected = false;
  ball_counter = 0;
  
  // Calibrate the LDR threshold baseline
  digitalWrite(LSR_PIN, HIGH);
  delay(1000);
  sensor.calibrate();
  
  // Start the puzzle
  digitalWrite(SIG_PIN, LOW);
  Serial.println(F(" -- Arduino Start -- "));
  Serial.print(F("Number of Balls: "));
  Serial.println(max_number_balls);
  Serial.print(F("LDR Threshold: "));
  Serial.println(sensor.threshold);
}
void(*resetFunc)(void) = 0;

/* Description: Main program loop */
void loop() {
  if(digitalRead(SIG_PIN) == HIGH) return;

  // Check if the proximity sensor is LOW
  if(digitalRead(PROX_PIN) == 0 && !prox_triggered){ 
    Serial.println("Proximity triggered..");
    prox_triggered = true; 
  }

  // Check if the LDR sensor is HIGH
  if(sensor.isHigh() && !ball_detected){
    Serial.println(F("Ball detected.."));
    ball_detected = true; 
    // Delay to prevent double-counting
    delay(L_DELAY);
  }

  // Check if the proximity sensor is active
  if(prox_triggered && ball_detected){
    // Update the Serial output
    Serial.print(F(" LDR Value: "));
    Serial.print(sensor.value);
    Serial.print(F(" Balls Counted: "));
    Serial.println(++ball_counter);
    prox_triggered = false;
    ball_detected = false;
  }

  // Check for puzzle completion
  if(ball_counter >= max_number_balls){
    Serial.println(F("Releasing Maglock..."));
    delay(1000);
    digitalWrite(SIG_PIN, HIGH);
    Serial.println(F(" -- Control Panel (R) Unlocked! -- "));
    Serial.println();
  }
}
