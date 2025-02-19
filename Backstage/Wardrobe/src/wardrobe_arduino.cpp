#include "Arduino.h"
const int maglockpin = 8; // Maglock pin
const int reedpin = A2; // Reed Swith pin

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(maglockpin, OUTPUT); // Set the pin to output so it send voltagte
  digitalWrite(maglockpin, HIGH);   // turn the Maglock on (HIGH is the voltage level)
  pinMode(reedpin, INPUT); // Set pin to input so it detects voltage
}

// the loop function runs over and over again forever
void loop() {
  Serial.begin(9600);
  if (digitalRead(reedpin) == 1) {
    Serial.println("Reedpin");
    digitalWrite(maglockpin, LOW);
  }
}
