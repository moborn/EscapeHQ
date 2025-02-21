 /*
 *   rDate: July 2020
 *  Author: Matt Cassidy 
 * 
 *  Title:  Farm Puzzles v1.0
 *  Board:  Arduino Uno r3
 *  Brief:  2x Puzzles designed for use in EscapeHQ game rooms
 *           A. Milk Puzzle - Float switch/sensor to release maglock
 *           B. Tractor Puzzle - Load Cell & weight sensor module to release maglock
 *          Signal pins (maglock) are LOW until puzzles are solved, then HIGH
 *  
 *  Notes:  Imports "HX711 Arduino Library" by Bogdan Necula & Andreas Motl
 * 
 * Copyright 2020, mattc.creativesolutions@gmail.com
 */
#include "HX711.h"
#include <Arduino.h>

#define COM_PIN A0    // <-- Houdini Communication (reserved)
#define SIG_PIN 11    // <-- Signal pin ("left panel" maglock)
#define SIG_OUT 12    // <-- Signal pin ("exit door" maglock)
#define FLT_PIN 10    // <-- Float switch/sensor
#define HX711_DTX 9   // <-- HX711 Digital Out
#define HX711_CLK 8   // <-- HX711 Signal Clock
#define L_DELAY 1000  // <-- Loop iteration delay (ms)

static const int16_t calibration_factor = 2280.f;
static const uint8_t weight_threshold = 20.0;
static const uint8_t min_item_weight = 7.0;
static const uint8_t sampling_factor = 5;
long weigh_table_reading;
HX711 weigh_table;

/* Description: Startup */
void setup() {
  Serial.begin(9600);
  pinMode(SIG_OUT, OUTPUT);
  pinMode(SIG_PIN, OUTPUT);
  pinMode(FLT_PIN, INPUT_PULLUP);

  // Initialise the HX711 weight sensor module
  weigh_table.begin(HX711_DTX, HX711_CLK);
  weigh_table.set_scale(calibration_factor);
  weigh_table.tare();

  // Start the puzzle
  weigh_table_reading = 0.0;
  digitalWrite(SIG_OUT, LOW);
  digitalWrite(SIG_PIN, LOW);
  Serial.println(F(" -- Arduino Start -- "));
}
void(*resetFunc)(void) = 0;

/* Description: Main program loop */
void loop() { 
  if(digitalRead(SIG_OUT) == HIGH) return;
  
  // Check the float switch/sensor
  if(digitalRead(FLT_PIN) == HIGH && digitalRead(SIG_PIN) == LOW)
  {
    Serial.println(F(" -- Float Switch Complete -- "));
    digitalWrite(SIG_PIN, HIGH);
  }

  // Wake up the HX711 ADC
  //weigh_table.power_up();
  //delay(100);

  // Check the HX711 weight sensor for new weight readings
  if (weigh_table.is_ready()) {
    long reading = weigh_table.get_units(sampling_factor);
    if(reading >= weigh_table_reading + min_item_weight)
      weigh_table_reading += reading;
    Serial.print("HX711 reading (averaged): ");
    Serial.println(reading);
  } else {
    Serial.println("Error! HX711 unavailable");
  }

  // Check for Tractor puzzle completion
  if(weigh_table_reading >= weight_threshold){
    digitalWrite(SIG_OUT, HIGH);
    Serial.println(F(" -- Puzzle Complete -- "));
  }

  // Put the HX711 ADC into sleep mode
  //weigh_table.power_down();
  
  // Limit loop iterations
  delay(L_DELAY);
}
