/*
 * Author: Matt Cassidy
 * Date: 12/05/2020
 * 
 * Brief: Keypad Door-Entry program designed for use in the EscapeHQ puzzle rooms.
 *        (Imports "Keypad" library by Mark Stanley & Alexander Brevig)
 * 
 * Instructions:
 *    1.  Set the desired passcode using Passcode[] field
 *    2.  Set the passcode length using P_LENGTH field
 *    3.  Signal pin (maglock) is HIGH until puzzle is solved, then LOW
 * 
 * Copyright 2020, mattc.creativesolutions@gmail.com
 */
#include <Keypad.h>

#define P_LENGTH 4                    // <-- Set Passcode Length

int signalPin = 12;
char MasterKey = '*';
char Passcode[P_LENGTH+1] = "5913";   // <-- Default Passcode
char Key_Data[P_LENGTH+1]; 
char KeyPressed;
byte count = 4;

// Configure Keypad library and pinouts
const byte cROWS = 4;
const byte cCOLS = 3;
char Keys[cROWS][cCOLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[cROWS] = {8, 7, 6, 5};
byte colPins[cCOLS] = {4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(Keys), rowPins, colPins, cROWS, cCOLS);


void setup() {
  Serial.begin(9600);
  pinMode(signalPin, OUTPUT);
  digitalWrite(signalPin, HIGH);
}

void loop() {
  // Poll for Keypad activity
  KeyPressed = customKeypad.getKey(); 

  // Validate key presses
  if (KeyPressed){
    Serial.println(KeyPressed);
    if (KeyPressed == MasterKey){
      // Clear key presses
      while(count != 0){ Key_Data[count--] = 0; }
    } else {
      // Store key presses
      Key_Data[count] = KeyPressed; 
      count++;  
    }
  }

  // Check for valid passcode
  if(count == P_LENGTH){
    Serial.println(Key_Data);

    if(strcmp(Key_Data, Passcode) == 0){
      delay(500);
      // Disable maglock
      digitalWrite(signalPin, LOW);
    }

    // Clear stored key presses
    while(count != 0){ Key_Data[count--] = 0; }
  }
  
}
