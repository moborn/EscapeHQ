/*
 *   rDate: June 2020
 *  Author: Matt Cassidy 
 * 
 *  Title:  Cow Insemination Puzzle v1.0
 *  Board:  Arduino Uno r3
 *  Brief:  Pullup Button with Audio designed for use in EscapeHQ game rooms
 *          Signal pin (maglock) is LOW until puzzle is solved, then HIGH
 *  
 *  Notes:  Puzzle is complete when pullup button reads LOW
 *          Imports "VS1053 for use with SdFat" by Michael P. Flaga & Bill Porter
 * 
 *  Audio:  track001.mp3  (triggered by proximity sensor)
 *          track002.mp3  (triggered by pullup button)
 * 
 * Copyright 2020, mattc.creativesolutions@gmail.com
 */
#include <vs1053_SdFat.h>
#include <SdFat.h>
#include <Arduino.h>

#define SIG_PIN 5    // <-- Signal pin (maglock)
#define COM_PIN A0    // <-- Houdini Communication (reserved)
#define PROD_PIN A1    // <-- Button input
#define PROX_PIN 10    // <-- E18-D8ONK Proximity Sensor
#define MONO_SET 1    // <-- VS1053 Player mono setting (0=off, 3=max)
#define MONO_VOL 1    // <-- VS1053 Player mono volume (255=off, 0=max)
#define TRACK_01 1    // <-- Index of track001.mp3
#define TRACK_02 2    // <-- Index of track002.mp3
#define INTERVAL 2    // <-- Interval (seconds) between repeating "Moo" audio
#define L_DELAY 100   // <-- Loop iteration delay (ms)

SdFat sd;
vs1053 MP3player;
uint8_t count = INTERVAL*10;

/* Description: Startup */
void setup() {
  Serial.begin(9600);
  pinMode(SIG_PIN, OUTPUT);
  pinMode(PROD_PIN, INPUT_PULLUP);
  pinMode(PROX_PIN, INPUT_PULLUP);

  // Wait for Serial Port to connect..
  while(!Serial){;}

  // Initialise the VS1053 Shield
  init_sdcard();
  init_mp3player();
  Serial.println();

  // Start the Puzzle
  digitalWrite(SIG_PIN, LOW);
  Serial.println(F(" -- Arduino Start -- "));
  Serial.print(F("Mono Setting: "));
  Serial.println(MONO_SET);
  Serial.print(F("Mono Volume:  "));
  Serial.println(MONO_VOL);
}
void(*resetFunc)(void) = 0;

/* Description: initializes the SD card and checks for errors */
void init_sdcard()
{
  // Initialise SDFat card
  Serial.print(F("Initializing SD card... "));
  byte result = 0;
  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
    result = 1;
  }
  if(!sd.chdir("/")) {
    sd.errorHalt("sd.chdir");
    result = 2;
  }
  if(result){
    Serial.print(F("Error code: "));
    Serial.println(result);
    delay(1000);
    resetFunc(); // Reset Arduino
  } else {
    Serial.println(F("Complete."));
    sd.open("/");
  }
}

/* Description: initializes the mp3 player and checks for errors */
void init_mp3player()
{
  Serial.print(F("Initializing VS1053 chip... "));
  uint8_t result = MP3player.begin();
  if(result){
    if(result == 6){
      Serial.println(F("Warning: patch file not found - skipping"));
    } else {
      Serial.print(F("Error code: "));
      Serial.println(result);
      delay(1000);
      resetFunc(); // Reset Arduino
    }
  }
  MP3player.setMonoMode(MONO_SET);
  MP3player.setVolume(MONO_VOL, MONO_VOL);
  Serial.println(F("Complete."));
}

/* Description: Main program loop */
void loop() {
  if(digitalRead(SIG_PIN) == HIGH) return;
  
  // Check proximity sensor for activity
  if(digitalRead(PROX_PIN) == 1 && count > 0) count--;
  if(digitalRead(PROX_PIN) == 0 && !count){
    while(MP3player.isPlaying()){;}
    Serial.println(F("Playing track001.mp3..."));
    MP3player.playTrack(TRACK_01);
    count = INTERVAL*10;
  }

  // Check for puzzle completion
  if(digitalRead(PROD_PIN) == LOW){
    while(MP3player.isPlaying()){;}
    Serial.println(F("Playing track002.mp3..."));
    MP3player.playTrack(TRACK_02);
    while(MP3player.isPlaying()){;}
    pinMode(SIG_PIN, OUTPUT);
    digitalWrite(SIG_PIN, HIGH);
    Serial.println(F(" -- Baby Cow Unlocked! -- "));
  }

  // Limit loop iterations
  delay(L_DELAY);
}
