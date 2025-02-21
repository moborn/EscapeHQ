/**
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from more than one PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 *
 * Example sketch/program showing how to read data from more than one PICC (that is: a RFID Tag or Card) using a
 * MFRC522 based RFID Reader on the Arduino SPI interface.
 *
 * Warning: This may not work! Multiple devices at one SPI are difficult and cause many trouble!! Engineering skill
 *          and knowledge are required!
 *
 * @license Released into the public domain.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS 1    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required **
 * SPI SS 2    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required **
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 *
 */

//------------------------------------------------------------------------------------Audio-------------------------------------------------------------------------

#include <Arduino.h>

#ifndef USE_SOFTWARESERIAL
#define USE_SOFTWARESERIAL 1   ///< Set to 1 to use SoftwareSerial library, 0 for native serial port
#endif

#include <MD_YX5300.h>
#include <MD_cmdProcessor.h>

#if USE_SOFTWARESERIAL
#include <SoftwareSerial.h>

// Connections for serial interface to the YX5300 module
const uint8_t ARDUINO_RX = 2;    // connect to TX of MP3 Player module
const uint8_t ARDUINO_TX = 3;    // connect to RX of MP3 Player module

SoftwareSerial  MP3Stream(ARDUINO_RX, ARDUINO_TX);  // MP3 player serial stream for comms
#define Console Serial           // command processor input/output stream
#else
#define MP3Stream Serial2  // Native serial port - change to suit the application
#define Console   Serial   // command processor input/output stream
#endif

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define CMD(s) { Console.print(F("\n>")); Console.print(F(s)); Console.print(F(" ")); }

// Define YX5300 global variables
MD_YX5300 mp3(MP3Stream);
bool bUseCallback = true; // use callbacks?
bool bUseSynch = false;   // use synchronous? 

void cbResponse(const MD_YX5300::cbData *status)
// Used to process device responses either as a library callback function
// or called locally when not in callback mode.
{
  if (bUseSynch)
    Console.print(F("\nSync Status: "));
  else
    Console.print(F("\nCback status: "));

  switch (status->code)
  {
  case MD_YX5300::STS_OK:         Console.print(F("STS_OK"));         break;
  case MD_YX5300::STS_TIMEOUT:    Console.print(F("STS_TIMEOUT"));    break;
  case MD_YX5300::STS_VERSION:    Console.print(F("STS_VERSION"));    break;
  case MD_YX5300::STS_CHECKSUM:   Console.print(F("STS_CHECKSUM"));    break;
  case MD_YX5300::STS_TF_INSERT:  Console.print(F("STS_TF_INSERT"));  break;
  case MD_YX5300::STS_TF_REMOVE:  Console.print(F("STS_TF_REMOVE"));  break;
  case MD_YX5300::STS_ERR_FILE:   Console.print(F("STS_ERR_FILE"));   break;
  case MD_YX5300::STS_ACK_OK:     Console.print(F("STS_ACK_OK"));     break;
  case MD_YX5300::STS_FILE_END:   Console.print(F("STS_FILE_END"));   break;
  case MD_YX5300::STS_INIT:       Console.print(F("STS_INIT"));       break;
  case MD_YX5300::STS_STATUS:     Console.print(F("STS_STATUS"));     break;
  case MD_YX5300::STS_EQUALIZER:  Console.print(F("STS_EQUALIZER"));  break;
  case MD_YX5300::STS_VOLUME:     Console.print(F("STS_VOLUME"));     break;
  case MD_YX5300::STS_TOT_FILES:  Console.print(F("STS_TOT_FILES"));  break;
  case MD_YX5300::STS_PLAYING:    Console.print(F("STS_PLAYING"));    break;
  case MD_YX5300::STS_FLDR_FILES: Console.print(F("STS_FLDR_FILES")); break;
  case MD_YX5300::STS_TOT_FLDR:   Console.print(F("STS_TOT_FLDR"));   break;
  default: Console.print(F("STS_??? 0x")); Console.print(status->code, HEX); break;
  }

  Console.print(F(", 0x"));
  Console.print(status->data, HEX);
}

void setCallbackMode(bool b)
{
  bUseCallback = b;
  CMD("Callback");
  Console.print(b ? F("ON") : F("OFF"));
  mp3.setCallback(b ? cbResponse : nullptr);
}

void setSynchMode(bool b)
{
  bUseSynch = b;
  CMD("Synchronous");
  Console.print(b ? F("ON") : F("OFF"));
  mp3.setSynchronous(b);
}


char * getNum(char *cp, uint32_t &v, uint8_t base = 10)
{
  char* rp;

  v = strtoul(cp, &rp, base);

  return(rp);
}

// Command processor handlers
void handlerHelp(char* param);

void handlerP_bang(char* param) { CMD("Play Start"); mp3.playStart(); cbResponse(mp3.getStatus()); }
void handlerPP(char* param)     { CMD("Play Pause"); mp3.playPause(); cbResponse(mp3.getStatus()); }
void handlerPZ(char* param)     { CMD("Play Stop");  mp3.playStop();  cbResponse(mp3.getStatus()); }
void handlerP_gt(char* param)   { CMD("Play Next");  mp3.playNext();  cbResponse(mp3.getStatus()); }
void handlerP_lt(char* param)   { CMD("Play Prev");  mp3.playPrev();  cbResponse(mp3.getStatus()); }

void handlerP(char* param)
{
  uint32_t t;
  
  getNum(param, t);
  CMD("Play Track");
  Console.print(t);
  mp3.playTrack(t);
  cbResponse(mp3.getStatus());
}

void handlerPT(char* param)
{
  uint32_t fldr, file;

  param = getNum(param, fldr);
  getNum(param, file);
  CMD("Play Specific Fldr");
  Console.print(fldr);
  Console.print(F(", "));
  Console.print(file);
  mp3.playSpecific(fldr, file);
  cbResponse(mp3.getStatus());
}

void handlerPF(char* param)
{
  uint32_t fldr;

  getNum(param, fldr);
  CMD("Play Folder");
  Console.print(fldr);
  mp3.playFolderRepeat(fldr);
  cbResponse(mp3.getStatus());
}

void handlerPX(char* param)
{
  uint32_t fldr;
  
  getNum(param, fldr);
  CMD("Play Shuffle Folder");
  Console.print(fldr);
  mp3.playFolderShuffle(fldr);
  cbResponse(mp3.getStatus());
}

void handlerPR(char* param)
{
  uint32_t file;

  getNum(param, file);
  CMD("Play File repeat");
  Console.print(file);
  mp3.playTrackRepeat(file);
  cbResponse(mp3.getStatus());
}


void handlerVM(char *param)
{
  uint32_t cmd;
  
  getNum(param, cmd);
  CMD("Volume Enable");
  Console.print(cmd);
  mp3.volumeMute(cmd != 0);
  cbResponse(mp3.getStatus());
}

void handlerV(char *param)
{
  uint32_t v;
  
  getNum(param, v);
  CMD("Volume"); 
  Console.print(v);
  mp3.volume(v); 
  cbResponse(mp3.getStatus());
}

void handlerV_plus(char* param)  { CMD("Volume Up");   mp3.volumeInc(); cbResponse(mp3.getStatus()); }
void handlerV_minus(char* param) { CMD("Volume Down"); mp3.volumeDec(); cbResponse(mp3.getStatus()); }

void handlerQE(char* param) { CMD("Query Equalizer");    mp3.queryEqualizer();   cbResponse(mp3.getStatus()); }
void handlerQF(char* param) { CMD("Query File");         mp3.queryFile();        cbResponse(mp3.getStatus()); }
void handlerQS(char* param) { CMD("Query Status");       mp3.queryStatus();      cbResponse(mp3.getStatus()); }
void handlerQV(char* param) { CMD("Query Volume");       mp3.queryVolume();      cbResponse(mp3.getStatus()); }
void handlerQX(char* param) { CMD("Query Folder Count"); mp3.queryFolderCount(); cbResponse(mp3.getStatus()); }
void handlerQY(char* param) { CMD("Query Tracks Count"); mp3.queryFilesCount();  cbResponse(mp3.getStatus()); }

void handlerQZ(char* param)
{
  uint32_t fldr;
  
  getNum(param, fldr);
  CMD("Query Folder Files Count");
  Console.print(fldr);
  mp3.queryFolderFiles(fldr);
  cbResponse(mp3.getStatus());
}

void handlerS(char *param) { CMD("Sleep");   mp3.sleep();  cbResponse(mp3.getStatus()); }
void handlerW(char *param) { CMD("Wake up"); mp3.wakeUp(); cbResponse(mp3.getStatus()); }
void handlerZ(char *param) { CMD("Reset");   mp3.reset();  cbResponse(mp3.getStatus()); }

void handlerE(char *param)
{
  uint32_t e;

  getNum(param, e);
  CMD("Equalizer");
  Console.print(e);
  mp3.equalizer(e);
  cbResponse(mp3.getStatus());
}

void handlerX(char *param)
{
  uint32_t cmd;

  getNum(param, cmd);
  CMD("Shuffle");
  Console.print(cmd);
  mp3.shuffle(cmd != 0);
  cbResponse(mp3.getStatus());
}

void handlerR(char* param)
{
  uint32_t cmd;

  getNum(param, cmd);
  CMD("Repeat");
  Console.print(cmd);
  mp3.repeat(cmd != 0);
  cbResponse(mp3.getStatus());
}

void handlerY(char *param)
{
  uint32_t cmd;
  
  getNum(param, cmd);
  setSynchMode(cmd != 0);
}

void handlerC(char * param)
{
  uint32_t cmd;

  getNum(param, cmd);
  setCallbackMode(cmd != 0);
}

const MD_cmdProcessor::cmdItem_t PROGMEM cmdTable[] =
{
  { "?",  handlerHelp,    "",     "Help", 0 },
  { "h",  handlerHelp,    "",     "Help", 0 },
  { "p!", handlerP_bang,  "",     "Play", 1 },
  { "p",  handlerP,       "n",    "Play file index n (0-255)", 1 },
  { "pp", handlerPP,      "",     "Play Pause", 1 },
  { "pz", handlerPZ,      "",     "Play Stop", 1 },
  { "p>", handlerP_gt,    "",     "Play Next", 1 },
  { "p<", handlerP_lt,    "",     "Play Previous", 1 },
  { "pt", handlerPT,      "f n",  "Play Track folder f, file n", 1 },
  { "pf", handlerPF,      "f",    "Play loop folder f", 1 },
  { "px", handlerPX,      "f",    "Play shuffle folder f", 1 },
  { "pr", handlerPR,      "n",    "Play loop file index n", 1 },
  { "v+", handlerV_plus,  "",     "Volume up", 2 },
  { "v-", handlerV_minus, "",     "Volume down", 2 },
  { "v",  handlerV,       "x",    "Volume set x (0-30)", 2 },
  { "vm", handlerVM,      "b",     "Volume Mute on (b=1), off (0)", 2 },
  { "qe", handlerQE,      "",     "Query equalizer", 3 },
  { "qf", handlerQF,      "",     "Query current file", 3 },
  { "qs", handlerQS,      "",     "Query status", 3 },
  { "qv", handlerQV,      "",     "Query volume", 3 },
  { "qx", handlerQX,      "",     "Query folder count", 3 },
  { "qy", handlerQY,      "",     "Query total file count", 3 },
  { "qz", handlerQZ,      "f",    "Query files count in folder f", 3 },
  { "s",  handlerS,       "",     "Sleep", 4 },
  { "w",  handlerW,       "",     "Wake up", 4 },
  { "e",  handlerE,       "n",    "Equalizer type n", 5 },
  { "x",  handlerX,       "b",    "Play Shuffle on (b=1), off (0)", 5 },
  { "r",  handlerR,       "b",    "Play Repeat on (b=1), off (0)", 5 },
  { "z",  handlerZ,       "",     "Reset", 5 },
  { "y",  handlerY,       "b",    "Synchronous mode on (b=1), off (0)", 6 },
  { "c",  handlerC,       "b",    "Callback mode on (b=1), off (0)", 6 },
};

MD_cmdProcessor CP(Console, cmdTable, ARRAY_SIZE(cmdTable));

// handler functions
void handlerHelp(char* param)
{
  Console.print(F("\n[MD_YX5300 Test]\nSet Serial line ending to newline."));
  CP.help();
  Console.print(F("\n"));
}

//------------------------------------------------------------------------------------RFID-------------------------------------------------------------------------------

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_1_PIN        7         // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 2
#define SS_2_PIN        6          // Configurable, take a unused pin, only HIGH/LOW required, must be different to SS 1
#define SS_3_PIN        5

#define SIG_PIN 4     // <-- Signal pin (maglock)

#define NR_OF_READERS   3
#define NUM_REGISTERED_CARDS 3

byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN};
bool cardPresent[3];
bool masterTriggered[3];

MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

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
    Serial.print("\n");
  }
} new_card, master_card = {0xE5,0x79,0x45,0x1C};

RFID_UID registered_cards[NUM_REGISTERED_CARDS];
RFID_UID backup_cards[NUM_REGISTERED_CARDS];

int counter = 0;

bool allPresent(bool *arr){
  for(uint8_t i = 0; i < NUM_REGISTERED_CARDS; i++){
    if(!arr[i]) return false;
  }
  return true;
}

void completePuzzle(){
  pinMode(SIG_PIN, OUTPUT);
  mp3.volume(100);
  mp3.playStart();
  delay(1500);
  digitalWrite(SIG_PIN, HIGH);
  Serial.println(F(" -- Puzzle Complete -- "));
}

/**
 * Initialize.
 */
void setup() {

  Serial.begin(9600); // Initialize serial communications with the PC
  pinMode(SIG_PIN, OUTPUT);
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  SPI.begin();        // Init SPI bus
    // Add Registered RFID cards
  registered_cards[0] = {0x60,0xCC,0x22,0x1A};  // <-- Puzzle 0 60 CC 22 1A
  registered_cards[1] = {0xD5,0xC4,0x71,0x1C};  // <-- Puzzle 1 D5 C4 71 1C
  registered_cards[2] = {0x50,0x49,0xEF,0x1A};  // <-- Puzzle 2 50 49 EF 1A

  backup_cards[0] = {0xB0,0x48,0xED,0x1A}; // <-- Puzzle 0 Backup B0 48 ED 1A
  backup_cards[1] = {0x60,0xF4,0x68,0x1A}; // <-- Puzzle 1 Backup 60 F4 68 1A
  backup_cards[2] = {0xC0,0xAB,0x75,0x1A}; // <-- Puzzle 2 Backup C0 AB 75 1A

  //Readers Ordered 0 - 2 - 1 Left to right

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_SetAntennaGain(0x07<<4);
    mfrc522[reader].PCD_DumpVersionToSerial();
  }

  // YX5300 Serial interface
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);
  mp3.begin();
  setCallbackMode(bUseCallback);
  setSynchMode(bUseSynch);

  // command line interface
  Console.begin(9600);
  CP.begin();
  CP.help();
}

/**
 * Main loop.
 */
void loop() {

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      new_card.update(mfrc522[reader].uid.uidByte);
      cardPresent[reader] = new_card.compare(registered_cards[reader]) || new_card.compare(backup_cards[reader]) ? true : false;
      masterTriggered[reader] = new_card.compare(master_card);
      Serial.println();
      Serial.print(F("Reader "));
      Serial.print(reader);
      //Show some details of the PICC
      Serial.print(F(": Card UID:"));
      new_card.printToSerial();

      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    }else{
      cardPresent[reader] = false;
    }
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card to reset reader for subsequent read operations
  }

  counter = allPresent(cardPresent) ? counter + 1 : 0;
  if ((counter > 5 || masterTriggered[0] || masterTriggered[1] || masterTriggered[2]) && digitalRead(SIG_PIN) == LOW){
    Serial.println("Finished");
    completePuzzle();
  }
  
  CP.run();
  mp3.check();
  delay(100);
}
