#include "MatrixButton.h"
	

/// button code from here https://www.instructables.com/Touch-Tone-MIDI-Phone/
// https://github.com/poprhythm/Touch-Tone-Midi/blob/main/TouchToneMidi/TouchToneMidi.ino


// input-output pin combinations for all the keypad buttons
MatrixButton keypad[] = {
  {2, 6},    /* 1 */  {2, 7}, /* 2 */  {2, 8}, /* 3 */
  {3, 6},    /* 4 */  {3, 7}, /* 5 */  {3, 8}, /* 6 */
  {4, 6},    /* 7 */  {4, 7}, /* 8 */  {4, 8}, /* 9 */
  {10, 6}, /* * */  {5, 7}, /* 0 */  {10, 8}  /* # */
};

static byte lastKey = 255;
static byte count = 0;

//currently only stores 3 digit code, so every 3 button presses
int code[] = {0,0,0};
int codeIndex = 0;

static int correctCode[] = {5,5,7};

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
      // anyPressed = true;
      // if (i == 4){
      //   Serial.println("5");
      // }
      // else if (i == 6){
      //   Serial.println("7");
      // }
      
      

      // this loop here should protect against floating pins, or whatevers going on in other phone circuitry
      //count is currently arbitrary, but should be enough consecutive 'pressed' states to be sure that button press is real

      if (i == lastKey) {
        count++;
        if (count == 10) {
          // Serial.print("Key ");
          Serial.print(i + 1);
          // Serial.println(" pressed 10 times in a row");
          count = 0; // reset count after printing

          //add pressed number to stored code
          code[codeIndex] = i + 1;
          codeIndex++;

          //if code is 3 digits long, print it
          //this is where the code is actually used

          if (codeIndex == 3){
            Serial.print("Code: ");
            for (int i = 0; i < 3; i++){
              Serial.print(code[i]);
            }
            Serial.println();
            for (int i = 0; i < 3; i++){
              if (code[i] != correctCode[i]){
                break;
                Serial.println("Code incorrect");
              }
              if (i == 2){
                Serial.println("Code correct");
              }
            }
            codeIndex = 0;
          }

        }
      } else {
        lastKey = i;
        count = 1;
      }


      
      delay(10);
      }

    // else if (state == MatrixButton::RELEASED && hasChanged){
    //   Serial.print("Key ");
    //   Serial.print(i);
    //   Serial.println(" released");
    // }
  }
}
