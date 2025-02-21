#include <Arduino.h>

#define BUTTON 8  // Push button that unlocks the MGPIN1
#define MGPIN1 6  // Pins for the maglock -> residue from linear actuator
#define MGPIN2 5  //for auck 5, ham 5
#define STBY 3    //for auck 3, ham 3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Setting up the pin modes
  pinMode(BUTTON, INPUT);
  pinMode(MGPIN1, OUTPUT);
  pinMode(MGPIN2, OUTPUT);

  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  // Both pins to low so relay is off
  digitalWrite(MGPIN1, LOW);
  digitalWrite(MGPIN2, LOW);
}

void loop() {

  delay(30);
  int but = digitalRead(BUTTON);

  Serial.print("Button Pressed?  ");
  Serial.println(but);

  if (but == HIGH) {
    delay(100);  // Debounce - make sure it's pressed
    but = digitalRead(BUTTON);
    if (but == HIGH) {
      Serial.print("Unlocking Maglock!");
      digitalWrite(MGPIN2, HIGH);  // Make 1 pin high so relay is on
      digitalWrite(MGPIN1, HIGH);  // Make 1 pin high so relay is on
    }
  }
}
