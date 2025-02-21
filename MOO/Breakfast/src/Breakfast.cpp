/*
 *   rDate: June 2020
 *  Author: Matt Cassidy 
 * 
 *  Title:  Skip Breakfast Puzzle v1.1
 *  Board:  Arduino Nano v3
 *  Brief:  RFID card reader designed for use in the EscapeHQ puzzle rooms
 *          Signal pin (maglock) is HIGH until puzzle is solved, then LOW
 *  
 *  Cards:  RFID# E579451C = Master Reset
 *          RFID# 50FEDC1A = Unlock Card
 *          RFID# FFFFFFFF = Backup Card
 *  
 *  Notes:  Puzzle is complete when a listed RFID card (above) is read
 *          Imports "MFRC522" library by miguelbalboa
 * 
 *  SPI pin layout:
 *          ------------------------------------
 *                      MFRC522      Arduino    
 *                      Reader/PCD   Nano v3    
 *          Signal      Pin          Pin        
 *          ------------------------------------
 *          RST/Reset   RST          D09         
 *          SPI SS      SDA(SS)      D10        
 *          SPI MOSI    MOSI         D11        
 *          SPI MISO    MISO         D12        
 *          SPI SCK     SCK          D13        
 * 
 * Copyright 2020, mattc.creativesolutions@gmail.com
 */
#include <MFRC522.h>
#include <Arduino.h>
#include <SPI.h>

#define SIG_PIN 2     // <-- Signal pin (maglock)
#define L_DELAY 1000  // <-- Loop iteration delay (ms)

const static byte mfrc522_rst = 9;
const static byte mfrc522_sda = 10;
const static byte num_registered_cards = 2;

/* RFID UID structure */
struct RFID_UID{
  const static byte size = 4;
  byte uid[size];
  void update(byte uid[size]){
    for(byte i = 0; i < size; i++)
      this->uid[i] = uid[i];
  }
  void erase(){
    for(byte i = 0; i < size; i++)
      uid[i] = 0xFF;
  }
  bool copy(RFID_UID card){
    for(byte i = 0; i < size; i++)
      this->uid[i] = card.uid[i];
  }
  bool compare(RFID_UID card){
    for(byte i = 0; i < size; i++){
      if(this->uid[i] != card.uid[i]) return false;
    }
    return true;
  }
  void printToSerial(){
    for (byte i = 0; i < size; i++) 
      Serial.print(this->uid[i], HEX);
  }
} new_card, master_card = {0xE5,0x79,0x45,0x1C};
RFID_UID registered_cards[num_registered_cards];

/* MFRC522 interface */
MFRC522 mfrc522(mfrc522_sda, mfrc522_rst);

/* Description: Startup */
void setup() {
  Serial.begin(9600);
  pinMode(SIG_PIN, OUTPUT);

  // Initialise MFRC522 interface
  while(!Serial);
  SPI.begin();
  mfrc522.PCD_Init();

  // Add Registered RFID cards
  registered_cards[0] = {0x50,0xFE,0xDC,0x1A};  // <-- Default
  registered_cards[1] = {0xFF,0xFF,0xFF,0xFF};  // <-- Backup

  // Start the puzzle
  digitalWrite(SIG_PIN, HIGH);
  Serial.println(F(" -- Puzzle Start -- "));
  Serial.print(F("Master Card: "));
  master_card.printToSerial();
  Serial.print(F("\nUnlock Card: "));
  registered_cards[0].printToSerial();
  Serial.print(F("\nBackup Card: "));
  registered_cards[1].printToSerial();
  Serial.println();
}
void(*resetFunc)(void) = 0;

/* Description: Main program loop */
void loop() {
    
  // Check for new RFID cards
  if(!mfrc522.PICC_IsNewCardPresent()) return;
  if(!mfrc522.PICC_ReadCardSerial()) return;

  // Read the new RFID card
  new_card.update(mfrc522.uid.uidByte);
  Serial.print(F("Card UID: "));
  new_card.printToSerial();

  // Process the new RFID card
  if(digitalRead(SIG_PIN) == LOW)
  {    
    // Check for puzzle reset (Master Card)
    if(new_card.compare(master_card)){
      Serial.println(F(" -> Master Card Reset"));
      pinMode(SIG_PIN, OUTPUT);
      digitalWrite(SIG_PIN, HIGH);
      Serial.println(F(" -- Puzzle Reset -- "));
    }
  } else {    
    // Check for puzzle completion
    bool puzzle_complete = new_card.compare(master_card);
    for(byte i = 0; i < num_registered_cards; i++){
      if(new_card.compare(registered_cards[i])) 
        puzzle_complete = true;
    }
    if(puzzle_complete){
      Serial.println(" -> Access Granted");
      pinMode(SIG_PIN, OUTPUT);
      digitalWrite(SIG_PIN, HIGH);
      Serial.println(F(" -- Puzzle Complete -- "));
    } else {
      Serial.println(" -> Access Denied");
    }
  }
  
  // Ignore the rest of the RFID tag info
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  // Limit loop iterations
  delay(L_DELAY);
}
