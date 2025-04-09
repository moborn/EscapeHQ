#include "MatrixButton.h"
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>
#include <vs1053_SdFat.h>
	
/// button code from here https://www.instructables.com/Touch-Tone-MIDI-Phone/
// https://github.com/poprhythm/Touch-Tone-Midi/blob/main/TouchToneMidi/TouchToneMidi.ino
// input-output pin combinations for all the keypad buttons>
// this was the old (tested) arrangement, using all digital IO pins
// MatrixButton keypad[] = {
//   {5, 7},    /* 0 */  {2, 6}, /* 1 */  {2, 7}, /* 2 */  {2, 8}, /* 3 */
//   {3, 6},    /* 4 */  {3, 7}, /* 5 */  {3, 8}, /* 6 */
//   {4, 6},    /* 7 */  {4, 7}, /* 8 */  {4, 8}  /* 9 */
// };
//as MP3 board uses a large number of the pins, the buttonmapping matrix above cant be used. 
//MP3 board DOESN'T use analog pins, or digital pins 5+10
// analog pins can work as digital, A0 => 14, A1 => 15 etc
// so pins 5, 10, 14, 15, 16, 17, 18 are available for phone buttons
MatrixButton keypad[] = {
    {19, 19},    /* 0 */  {14, 15}, /* 1 */  {19, 15}, /* 2 */  {5, 15}, /* 3 */
    {14, 16},    /* 4 */  {19, 16}, /* 5 */  {5, 16}, /* 6 */
    {14, 17},    /* 7 */  {19, 17}, /* 8 */  {5, 17}  /* 9 */
  };
static byte lastKey = 255;
static byte count = 0;
//currently only stores 3 digit code, so every 3 button presses
int code[] = {0,0,0,0,0,0,0};
int codeIndex = 0;
static int correctCode[] = {4,8,9,4,9,9,9};
int hanger_pin = 3;
/**
 * \brief Object instancing the SdFat library.
 *
 * principal object for handling all SdCard functions.
 */
SdFat sd;
/**
 * \brief Object instancing the vs1053 library.
 *
 * principal object for handling all the attributes, members and functions for the library.
 */
vs1053 MP3player;
bool firstpress = false;
bool wrongnumber = false;
char dialtone[] = "dialtone.mp3";
char ring[] = "ring.mp3";
char correct_audio[] = "correct_num.mp3";
char hangup[] = "hangup.mp3";
char wrong[] = "wrong_num.mp3";
char dialtone_long[] = "dialtone_long.mp3";

int relaypin = 10;

void setup() {
  // initialize keypad button states
  Serial.begin(115200);
  for (byte i = 0; i < 12; i++){
      keypad[i].begin();
  }
    //i assume below if statements are incase of sd read error
    if(!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt();
    if (!sd.chdir("/")) sd.errorHalt("sd.chdir");
    MP3player.begin();
    MP3player.setVolume(0x0000,0x0000);  //0x0000 is max volume
  pinMode(hanger_pin, INPUT_PULLUP);
  pinMode(relaypin, OUTPUT);
  digitalWrite(relaypin, LOW);
  
}
void loop() {
  // Below is only needed if not interrupt driven. Safe to remove if not using. 
  //this was included in library examples, so may as well keep
    #if defined(USE_MP3_REFILL_MEANS) \
    && ( (USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer) \
    ||   (USE_MP3_REFILL_MEANS == USE_MP3_Polled)      )
    MP3player.available();
    #endif
  // MP3player.enableTestSineWave(50);
  
  if (digitalRead(hanger_pin) == LOW && wrongnumber == false){
  //  Serial.println("Hanger is off");
  // scan keypad for key presses
  if (firstpress == false && MP3player.isPlaying() == false){

    MP3player.playMP3(dialtone_long);
    Serial.println("Dialtone playing");
    
  }
  for (byte i = 0; i < 12; i++)
  {
    bool hasChanged;
    bool state = keypad[i].read(hasChanged);
    if (state == MatrixButton::PRESSED){
      // firstpress = true;
      // anyPressed = true;
      // if (i == 4){
      //   Serial.println("5");
      // }
      // else if (i == 6){
      //   Serial.println("7");
      // }
      
      
      // this loop here should protect against floating pins, or whatevers going on in other phone circuitry
      //count is currently arbitrary, but should be enough consecutive 'pressed' states to be sure that button press is real
      //this is fine and it works, but need to have something where it doesn't log multiple times if you hold button down
      //this is very picky to add that in, so not top priority, but may as well.
      if (i == lastKey) {
        count++;
        if (count == 10) {
          firstpress = true;
          MP3player.stopTrack();//this stops the dialtone
          // Serial.print("Key ");
          Serial.print(i);
          // Serial.println(" pressed 10 times in a row");
          count = 0; // reset count after printing
          //add pressed number to stored code
          code[codeIndex] = i;
          codeIndex++;

          // Serial.println(codeIndex);
          //play dial tone
          MP3player.playTrack(i);



          // Serial.println(codeIndex);

          //if code is 3 digits long, print it
          //this is where the code is actually used

          if (codeIndex == 3){
            Serial.println();
            Serial.print("Code: ");
            for (int i = 0; i < 3; i++){
              Serial.print(code[i]);
            }
            Serial.println();
            // Serial.print();
            for (int i = 0; i < 3; i++){
              if (code[i] != correctCode[i]){
                MP3player.playMP3(ring);
                delay(10000);
                MP3player.playMP3(wrong);
                delay(7500);
                MP3player.playMP3(hangup);
                wrongnumber = true;
                break;
                // Serial.println("Code incorrect");
              }
              if (i == 2){
                Serial.println("Code correct");
                // Serial.println(" is correct!");
                MP3player.playMP3(ring);
                delay(10000); //let track play fully and then end
                MP3player.playMP3(correct_audio);
                delay(7500); 
                MP3player.playMP3(hangup);
                delay(10000);
                digitalWrite(relaypin, HIGH);
                delay(1000);
                digitalWrite(relaypin, LOW);
              }
            }
            codeIndex = 0;
          }
        }
      } else {
        lastKey = i;
        count = 1;
      }
      
      delay(15);
      }
    // else if (state == MatrixButton::RELEASED && hasChanged){
    //   Serial.print("Key ");
    //   Serial.print(i);
    //   Serial.println(" released");
    // }
  }
}
  else if (digitalRead(hanger_pin) == HIGH){
    // Serial.println("Hanger is on");
    // Serial.println(codeIndex);
    MP3player.stopTrack();
    firstpress = false;
    codeIndex = 0;
    wrongnumber = false;

  }
  else{
    // Serial.println("Hanger is off, but wrong number");
  }
}