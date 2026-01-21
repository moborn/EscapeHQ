#include <Arduino.h>
#define length(array) int(sizeof(array)/sizeof(array[0]))

int Rin = 2;
int Gin = 4;
int Bin = 6;
int Yin = 8;
int rpi_control = 10;


#define SIG_PIN 12
#define INPUT_A 2
#define INPUT_B 4
#define INPUT_C 6
#define INPUT_D 8

#define CHAR_A 'R'    // <-- Matched to pole colour
#define CHAR_B 'G'    // <-- Matched to pole colour
#define CHAR_C 'B'    // <-- Matched to pole colour
#define CHAR_D 'Y'    // <-- Matched to pole colour


int easy_order[] = {1,3,2,4};
int hard_order[] = {4,2,3,1,4,2,3};

char easy_sequence[4] = {'R','B','G','Y'};
uint16_t easys_length = 4;
uint16_t s_index = 0;

char hard_sequence[7] = {'Y','G','B','R','Y','G','B'};
uint16_t hard_length = 7;

/* Description: Startup */
void setup() {
 Serial.begin(9600);
 pinMode(INPUT_A, INPUT_PULLUP);
 pinMode(INPUT_B, INPUT_PULLUP);
 pinMode(INPUT_C, INPUT_PULLUP);
 pinMode(INPUT_D, INPUT_PULLUP);
 pinMode(SIG_PIN, OUTPUT);
 pinMode(rpi_control, INPUT_PULLUP);
 // Start the Puzzle
 digitalWrite(SIG_PIN, HIGH);
 Serial.println(F("\n -- Arduino Start -- "));
 Serial.print(F("Unlock Sequence: "));
//  for(int i = 0; i < s_length; i++){
  //  Serial.print(Sequence[i]);
  //  if (i < s_length-1) Serial.print(F(" -> "));
  //  else Serial.println();
//  }
}




void(*resetFunc)(void) = 0;

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

/* Description: Main program loop */
void loop() {
 if(digitalRead(SIG_PIN) == LOW) return;
 

  //if (digitalRead(rpi_control) == LOW), it should be in hard mode. meaning using hard_sequence and length
  if (digitalRead(rpi_control) == HIGH) {
    // Serial.println(F(" -- Hard Mode Active -- "));
    // Poll for button presses
    char Button = getActivity();  

    // Button Sequence = Y->B->G->R->Y->B->G
    if (Button == 0) return;
    if (Button == hard_sequence[s_index]) { s_index++; }
    else { 
      s_index = 0; 
      Serial.println(F(" -- Wrong Sequence! Resetting.. -- "));
      Serial.print(F("Unlock Sequence: "));
      for(int i = 0; i < hard_length; i++){
        Serial.print(hard_sequence[i]);
        if (i < hard_length-1) Serial.print(F(" -> "));
        else Serial.println();
      }
    }

    // Release maglock
    if (s_index == hard_length) {
      pinMode(SIG_PIN, OUTPUT);
      delay(1000);
      digitalWrite(SIG_PIN, LOW);
    }
  }
  // else if easy mode, (digitalRead(rpi_control) == HIGH)
  if (digitalRead(rpi_control) == LOW) {
    // Easy mode — fall through to the easy-sequence handling below
    // Poll for button presses
    // Serial.println(F(" -- Easy Mode Active -- "));
    char Button = getActivity();  

    // Button Sequence = R->B->G->Y
    if (Button == 0) return;
    if (Button == easy_sequence[s_index]) { s_index++; }
    else { 
      s_index = 0; 
      Serial.println(F(" -- Wrong Sequence! Resetting.. -- "));
      Serial.print(F("Unlock Sequence: "));
      for(int i = 0; i < easys_length; i++){
        Serial.print(easy_sequence[i]);
        if (i < easys_length-1) Serial.print(F(" -> "));
        else Serial.println();
      }
    }

    // Release maglock
    if (s_index == easys_length) {
      pinMode(SIG_PIN, OUTPUT);
      delay(1000);
      digitalWrite(SIG_PIN, LOW);
    }
  }
}







