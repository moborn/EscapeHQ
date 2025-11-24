#include <Arduino.h>
#include "SevSeg.h"

SevSeg sevseg;

// Set password to open door here
const char password[5] = "7298"; // 4-digit password + null terminator
// Setup variables for tracking code entry
char enteredCode[5] = "    ";    // 4 spaces + null terminator
byte digitCount = 0;

// Buzzer and relay pins
const byte BUZZER_PIN = 53;
const byte RELAY_PIN = 22;

// Buzzer tone frequencies
const int BUZZER_TONES_STARTING[] = {440, 880};
const int BUZZER_TONE_KEYPAD = 440;
const int BUZZER_TONES_NO[] = {523, 370}; // Descending augmented fourth (C to F#)
const int BUZZER_TONES_OPEN[] = {554, 659, 880}; // Ascending first inversion triad (C#, E, A)

// Comms pins
const byte OVERRIDE_PIN = 30; // If input received on this, regardless of current state, set state to open immediately.
const byte DOOR_OPEN_PIN = 31; // Send signal high on this to indicate to room RPi that this puzzle is complete.

// Keypad configuration
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte keypadRowPins[] = {42, 52, 50, 46};
byte keypadColPins[] = {44, 40, 48};

// Keypad state tracking
char lastKey = 0;
bool keyPressed = false;

// Timing
unsigned long previousUpdate = 0;
const long updateIntervalMs = 50;
long loopNum = 0;

// State machine
enum State { STATE_STARTING, STATE_INPUT, STATE_WAIT, STATE_OPEN, STATE_NO };
State currentState = STATE_STARTING;

// Wait state variables
long waitStateStartLoop = 0;

// Starting state variables
long startingStateStartLoop = 0;
bool startingBeepOn = false;

// NO state variables
byte flashCount = 0;
long noStateStartLoop = 0;
bool displayOn = true;
bool noSoundPlayed = false;

// OPEN state variables
byte openBeepCount = 0;
bool openBeepOn = false;
long openStateStartLoop = 0;

void setup() {
    byte numDigits = 4;  
    byte digitPins[] = {2, 5, 6, 8};
    byte segmentPins[] = {3,7,10,12,13,4,9,11};
    bool resistorsOnSegments = 0;
    bool updateWithDelays = false;
    bool leadingZeros = false;
    bool disableDecPoint = true;
    
    sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, 
                 resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
    // Brightness range from 200 to -200. Above 100 can flicker, 90 to be safe from flickering is still very bright and visible in daylight.
    sevseg.setBrightness(90); 
    
    startingStateStartLoop = 0; // Will start from loop 0
    
    // Set up keypad pins
    for (byte i = 0; i < ROWS; i++) {
        pinMode(keypadRowPins[i], OUTPUT);
        digitalWrite(keypadRowPins[i], HIGH);
    }
    for (byte i = 0; i < COLS; i++) {
        pinMode(keypadColPins[i], INPUT_PULLUP);
    }
    
    // Set up pins
    pinMode(OVERRIDE_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(DOOR_OPEN_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(DOOR_OPEN_PIN, LOW);
    
    sevseg.blank(); // Start with blank display (will be overridden by starting state)
}

void loop() {
    sevseg.refreshDisplay(); // Must run repeatedly

    // Check for override signal - if received, immediately go to OPEN state
    if (digitalRead(OVERRIDE_PIN) == HIGH && currentState != STATE_OPEN) {
        currentState = STATE_OPEN;
        openStateStartLoop = loopNum;
        openBeepCount = 0;
        openBeepOn = false;
    }

    unsigned long currentMillis = millis();

    if (currentMillis - previousUpdate >= updateIntervalMs) {
        previousUpdate = currentMillis;
        loopNum++;
        
        // State machine logic
        switch (currentState) {
            case STATE_STARTING:
                handleStartingState();
                break;
                
            case STATE_INPUT:
                handleInputState();
                break;
                
            case STATE_WAIT:
                handleWaitState();
                break;
                
            case STATE_OPEN:
                handleOpenState();
                break;
                
            case STATE_NO:
                handleNoState();
                break;
        }
    }
}

char scanKeypad() {
    char key = 0;
    bool currentlyPressed = false;
    
    // Scan each row
    for (byte row = 0; row < ROWS; row++) {
        // Set current row LOW, others HIGH
        for (byte i = 0; i < ROWS; i++) {
            digitalWrite(keypadRowPins[i], HIGH);
        }
        digitalWrite(keypadRowPins[row], LOW);
        
        // Small delay for signal to settle
        delayMicroseconds(10);
        
        // Check each column
        for (byte col = 0; col < COLS; col++) {
            if (digitalRead(keypadColPins[col]) == LOW) {
                key = keys[row][col];
                currentlyPressed = true;
                break;
            }
        }
        
        if (currentlyPressed) break;
    }
    
    // Set all rows back to HIGH
    for (byte i = 0; i < ROWS; i++) {
        digitalWrite(keypadRowPins[i], HIGH);
    }
    
    // Handle key press/release detection
    if (currentlyPressed && !keyPressed) {
        // New key press
        keyPressed = true;
        lastKey = key;
        return key;
    } else if (!currentlyPressed && keyPressed) {
        // Key released
        keyPressed = false;
        lastKey = 0;
    }
    
    return 0; // No new key press
}

void handleStartingState() {
    // Animation timing constants (all at half speed by multiplying by 2)
    const long PHASE1_DURATION = 20;  // Show 8888 (1000ms)
    const long SEGMENTS_PER_DIGIT = 7; // A, B, C, D, E, F, G
    const long PHASE2_DURATION = SEGMENTS_PER_DIGIT * 4 * 2; // Individual digit animations, doubled to allow for running animations at half loop speed
    const long BEEP_DURATION = 4; // 2 beeps * 2 loops per beep (on+off)
    
    const long PHASE2_START = PHASE1_DURATION;
    const long BEEP_START = PHASE2_START + PHASE2_DURATION;
    const long ANIMATION_END = BEEP_START + BEEP_DURATION;
    
    long elapsedLoops = loopNum - startingStateStartLoop;
    
    // Phase 1: Show 8888 to prove all lights are working
    if (elapsedLoops < PHASE2_START) {
        sevseg.setChars("8888");
    }
    // Phase 2: Individual digit animation (ending on middle segment G) to prove that pins are connected correctly
    else if (elapsedLoops < BEEP_START) {
        long actualLoop = elapsedLoops - PHASE2_START;
        // Only update display on even loops (for half speed)
        if (actualLoop % 2 == 0) {
            long animLoop = actualLoop / 2;
            byte digit = animLoop / SEGMENTS_PER_DIGIT;  // Which digit (0-3)
            byte segment = animLoop % SEGMENTS_PER_DIGIT; // Which segment (0-6)
            
            // Segment order: F, A, B, C, D, E, G
            // Bit positions: .GFEDCBA
            byte segmentCodes[7] = {
                0x20,  // F (top left)
                0x01,  // A (top)
                0x02,  // B (top right)
                0x04,  // C (bottom right)
                0x08,  // D (bottom)
                0x10,  // E (bottom left)
                0x40   // G (middle)
            };
            
            uint8_t segs[4] = {0, 0, 0, 0};
            if (digit < 4) {
                segs[digit] = segmentCodes[segment];
            }
            sevseg.setSegments(segs);
        }
    }
    // Phase 3: 2 beeps
    else if (elapsedLoops < ANIMATION_END) {
        long beepLoop = elapsedLoops - BEEP_START;
        if (beepLoop < 2) {
            tone(BUZZER_PIN, BUZZER_TONES_STARTING[0]);
        } else if (beepLoop < 4) {
            tone(BUZZER_PIN, BUZZER_TONES_STARTING[1]);
        }
        sevseg.blank(); // Keep display blank during beeps
    }
    // Animation complete, transition to input state
    else {
        noTone(BUZZER_PIN); // Make sure buzzer is off
        currentState = STATE_INPUT;
        digitCount = 0;
        for (byte i = 0; i < 4; i++) {
            enteredCode[i] = ' ';
        }
        sevseg.setChars("    ");
    }
}

void handleInputState() {
    char key = scanKeypad();
    
    if (key) {
        // Sound buzzer for one loop when button is pressed
        tone(BUZZER_PIN, BUZZER_TONE_KEYPAD);
        
        // Check if it's a digit (not *, #)
        //if (key >= '0' && key <= '9') {
            if (key == '*') key = 'r';
            if (key == '#') key = 'u';
            if (digitCount < 4) {
                enteredCode[digitCount] = key;
                digitCount++;
                sevseg.setChars(enteredCode);
                
                // Check if 4 digits have been entered
                if (digitCount == 4) {
                    // Enter wait state to display code before checking
                    currentState = STATE_WAIT;
                    waitStateStartLoop = loopNum;
                }
            }
        //}
    } else {
        // Turn off buzzer when no key is pressed
        noTone(BUZZER_PIN);
    }
}

void handleWaitState() {
    // Keep displaying the entered code
    sevseg.setChars(enteredCode);
    noTone(BUZZER_PIN);
    
    // After 4 loops, check the code and transition
    if (loopNum - waitStateStartLoop >= 4) {
        // Compare entered code with password
        bool match = true;
        for (byte i = 0; i < 4; i++) {
            if (enteredCode[i] != password[i]) {
                match = false;
                break;
            }
        }
        
        if (match) {
            currentState = STATE_OPEN;
            openStateStartLoop = loopNum;
            openBeepCount = 0;
            openBeepOn = false;
        } else {
            currentState = STATE_NO;
            flashCount = 0;
            noStateStartLoop = loopNum;
            displayOn = true;
            noSoundPlayed = false;
        }
    }
}

void handleOpenState() {
    // Activate relay and signal door open
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(DOOR_OPEN_PIN, HIGH);
    
    sevseg.setChars("OPEN");
    
    long elapsedLoops = loopNum - openStateStartLoop;
    
    // Play first inversion triad ending at 880Hz
    // Each tone plays for 2 loops
    if (elapsedLoops < 2) {
        tone(BUZZER_PIN, BUZZER_TONES_OPEN[0]);
    } else if (elapsedLoops < 4) {
        tone(BUZZER_PIN, BUZZER_TONES_OPEN[1]);
    } else if (elapsedLoops < 6) {
        tone(BUZZER_PIN, BUZZER_TONES_OPEN[2]);
    } else {
        // After triad, keep buzzer off
        noTone(BUZZER_PIN);
    }
}

void handleNoState() {
    long elapsedLoops = loopNum - noStateStartLoop;
    
    // Play descending augmented fourth once at the beginning
    // Each tone plays for 2 loops
    if (!noSoundPlayed && elapsedLoops < 4) {
        if (elapsedLoops < 2) {
            tone(BUZZER_PIN, BUZZER_TONES_NO[0]);
        } else {
            tone(BUZZER_PIN, BUZZER_TONES_NO[1]);
        }
        if (elapsedLoops == 3) {
            noSoundPlayed = true;
            noTone(BUZZER_PIN);
        }
    } else if (noSoundPlayed || elapsedLoops >= 4) {
        noTone(BUZZER_PIN);
    }
    
    long cyclePosition = elapsedLoops % 8; // 4 loops on, 4 loops off = 8 loops per cycle
    
    // Toggle display every 4 loops
    if (cyclePosition == 0) {
        displayOn = !displayOn;
        
        // Count completed flash cycles (on+off = one complete flash)
        if (!displayOn) {
            flashCount++;
        }
    }
    
    // Check if we've completed 3 flashes (6 toggles total)
    if (flashCount >= 3 && !displayOn) {
        // Reset to input state
        currentState = STATE_INPUT;
        digitCount = 0;
        for (byte i = 0; i < 4; i++) {
            enteredCode[i] = ' ';
        }
        sevseg.setChars("    ");
        noTone(BUZZER_PIN); // Make sure buzzer is off
    } else {
        // Display " NO " or blank
        if (displayOn) {
            sevseg.setChars(" NO ");
        } else {
            sevseg.setChars("    ");
        }
    }
}