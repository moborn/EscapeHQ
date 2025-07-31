#include <Arduino.h>

/*
* Author: Matt Cassidy 
* Date: October 2020
* 
* Brief: Ski Pole Button program designed for use in the EscapeHQ puzzle rooms.
*        Signal pin (maglock) is HIGH until puzzle is solved, then LOW.
*        Rewritten for Analog PULLUP pins
* 
* Instructions:
*    1.  Set the sequence using Sequence field
*    2.  Set the sequence length using s_length field
* 
* Copyright 2020, mattc.creativesolutions@gmail.com
*/

#define SIG_PIN 7
#define INPUT_A 8
#define INPUT_B 9
#define INPUT_C 10
#define INPUT_D 11
#define CHAR_A 'R'    // <-- Matched to pole colour
#define CHAR_B 'G'    // <-- Matched to pole colour
#define CHAR_C 'B'    // <-- Matched to pole colour
#define CHAR_D 'Y'    // <-- Matched to pole colour

char Sequence[4] = {'R','G','B','Y'};
uint16_t s_length = 4;
uint16_t s_index = 0;

/* Description: Startup */
void setup() {
 Serial.begin(9600);
 pinMode(INPUT_A, INPUT_PULLUP);
 pinMode(INPUT_B, INPUT_PULLUP);
 pinMode(INPUT_C, INPUT_PULLUP);
 pinMode(INPUT_D, INPUT_PULLUP);
 pinMode(SIG_PIN, OUTPUT);

 // Start the Puzzle
 digitalWrite(SIG_PIN, HIGH);
 Serial.println(F("\n -- Arduino Start -- "));
 Serial.print(F("Unlock Sequence: "));
 for(int i = 0; i < s_length; i++){
   Serial.print(Sequence[i]);
   if (i < s_length-1) Serial.print(F(" -> "));
   else Serial.println();
 }
}
void(*resetFunc)(void) = 0;

/* Description: Main program loop */
void loop() {
 if(digitalRead(SIG_PIN) == LOW) return;
 
 // Poll for button presses
 char Button = getActivity();  

 // Button Sequence = R->B->G->Y
 if (Button == 0) return;
 if (Button == Sequence[s_index]) { s_index++; }
 else { 
   s_index = 0; 
   Serial.println(F(" -- Wrong Sequence! Resetting.. -- "));
   Serial.print(F("Unlock Sequence: "));
   for(int i = 0; i < s_length; i++){
     Serial.print(Sequence[i]);
     if (i < s_length-1) Serial.print(F(" -> "));
     else Serial.println();
   }
 }

 // Release maglock
 if (s_index == s_length) {
   pinMode(SIG_PIN, OUTPUT);
   delay(1000);
   digitalWrite(SIG_PIN, LOW);
   Serial.println(F(" -- Ski Poles Unlocked! -- "));
 }
}

/* Description: Checks all buttons for new activity */
char getActivity() {
 char retval = 0;
 if (digitalRead(INPUT_A) == LOW){
   while (!digitalRead(INPUT_A)) retval = CHAR_A;
 }
 else if (digitalRead(INPUT_B) == LOW){
   while (!digitalRead(INPUT_B)) retval = CHAR_B;
 }
 else if (digitalRead(INPUT_C) == LOW){
   while (!digitalRead(INPUT_C)) retval = CHAR_C;
 }
 else if (digitalRead(INPUT_D) == LOW){
   while (!digitalRead(INPUT_D)) retval = CHAR_D;
 }
 delay(10);
 if (retval != 0) Serial.println(retval);
 return retval;
}
