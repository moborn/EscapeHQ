#include "MatrixButton.h"
	

/// button code from here https://www.instructables.com/Touch-Tone-MIDI-Phone/


// input-output pin combinations for all the keypad buttons
MatrixButton keypad[] = {
  {2, 6},    /* 1 */  {2, 7}, /* 2 */  {2, 8}, /* 3 */
  {3, 6},    /* 4 */  {3, 7}, /* 5 */  {3, 8}, /* 6 */
  {4, 6},    /* 7 */  {4, 7}, /* 8 */  {4, 8}, /* 9 */
  {10, 6}, /* * */  {5, 7}, /* 0 */  {10, 8}  /* # */
};

void setup() {
  // initialize keypad button states
  Serial.begin(9600);
  for (byte i = 0; i < 12; i++)
      keypad[i].begin();

}

void loop() {
  bool anyPressed = false;
  
  // scan keypad for key presses
  for (byte i = 0; i < 12; i++)
  {
    bool hasChanged;
    bool state = keypad[i].read(hasChanged);


    if (state == MatrixButton::PRESSED){
      anyPressed = true;
      // if (i == 4){
      //   Serial.println("5");
      // }
      // else if (i == 6){
      //   Serial.println("7");
      // }
      // if i is different from previous loop, print the number
      Serial.println(i+1);
      delay(100);
      }

    // else if (state == MatrixButton::RELEASED && hasChanged){
    //   Serial.print("Key ");
    //   Serial.print(i);
    //   Serial.println(" released");
    // }
  }
}
