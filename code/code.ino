#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDConsumerControl.h"
#include <EEPROM.h>
#include "esp_system.h"

USBHIDKeyboard Keyboard;
USBHIDConsumerControl Consumer;

// =====================
// Pin Definitions
// =====================
const int braillePins[6] = {6, 5, 4, 7, 15, 16};
const int numPadPins[6]  = {8, 18, 17, 9, 10, 11};
const int customKeys[6]  = {12, 13, 14, 20, 45, 3}; 

#define CUSTOM_MAIN_PIN 46
#define BACKSPACE_PIN   37
#define ENTER_PIN       42  
#define SPACE_PIN       38  
#define DELETE_PIN      39
#define PROGRAM_PIN     2
#define MODE_PIN        41
#define CAPSLOCK_PIN    21

// =====================
// States & Variables
// =====================
uint8_t braillePattern = 0;
bool brailleActive = false;
uint8_t numPattern = 0;
bool numActive = false;

// Braille Modifiers
bool nextIsCap = false;
bool nextIsNum = false;

uint8_t customMode = 0; 
bool processed[6] = {false}; 
bool specProcessed[5] = {false}; 

char programmedChars[3] = {' ', ' ', ' '}; 
bool isProgramming = false;
int keyToProgram = -1;

// =====================
// Robust HID Helpers
// =====================
void sendShortcut(char key) {
  Keyboard.press(KEY_LEFT_CTRL);
  delay(30); 
  Keyboard.press(key);
  delay(50);
  Keyboard.release(key); 
  delay(30);
  Keyboard.release(KEY_LEFT_CTRL);
}

void sendMedia(uint16_t code) {
  Consumer.press(code);
  delay(150); 
  Consumer.release();
}

struct BrailleMap {
  uint8_t pattern;
  char letter;
};

// Expanded Braille Table (Letters + Punctuation)
BrailleMap brailleTable[] = {
  {0b000001,'a'}, {0b000011,'b'}, {0b001001,'c'}, {0b011001,'d'},
  {0b010001,'e'}, {0b001011,'f'}, {0b011011,'g'}, {0b010011,'h'},
  {0b001010,'i'}, {0b011010,'j'}, {0b000101,'k'}, {0b000111,'l'},
  {0b001101,'m'}, {0b011101,'n'}, {0b010101,'o'}, {0b001111,'p'},
  {0b011111,'q'}, {0b010111,'r'}, {0b001110,'s'}, {0b011110,'t'},
  {0b100101,'u'}, {0b100111,'v'}, {0b111010,'w'}, {0b101101,'x'},
  {0b111101,'y'}, {0b110101,'z'},
  // Punctuation
  {0b000010,','}, {0b000110,';'}, {0b010010,':'}, {0b010110,'.'},
  {0b010100,'?'}, {0b010011,'!'}, {0b001100,'\"'},{0b000100,'\''},
  {0b100001,'-'}, {0b001100,'/'}, {0b011100,'@'}
};

char getBraille(uint8_t p) {
  for (auto &b : brailleTable) if (b.pattern == p) return b.letter;
  return 0;
}

char letterToNumber(char c) {
  if (c >= 'a' && c <= 'i') return (char)(c - 'a' + '1');
  if (c == 'j') return '0';
  return c;
}

// =====================
// SETUP
// =====================
void setup() {
  USB.begin();
  Keyboard.begin();
  Consumer.begin();
  EEPROM.begin(32);

  for(int i=0; i<3; i++) {
    programmedChars[i] = EEPROM.read(i);
    if (programmedChars[i] == 255 || programmedChars[i] == 0) programmedChars[i] = ' '; 
  }

  for (int i = 0; i < 6; i++) {
    pinMode(braillePins[i], INPUT_PULLUP);
    pinMode(numPadPins[i], INPUT_PULLUP);
    pinMode(customKeys[i], INPUT_PULLUP);
  }

  pinMode(BACKSPACE_PIN, INPUT_PULLUP);
  pinMode(ENTER_PIN, INPUT_PULLUP);
  pinMode(SPACE_PIN, INPUT_PULLUP);
  pinMode(DELETE_PIN, INPUT_PULLUP);
  pinMode(PROGRAM_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);
  pinMode(CAPSLOCK_PIN, INPUT_PULLUP);
}

// =====================
// LOOP
// =====================
void loop() {
  static bool modeWas = HIGH, progWas = HIGH;
  bool modeNow = digitalRead(MODE_PIN);
  bool progNow = digitalRead(PROGRAM_PIN);

  if (modeNow == LOW && modeWas == HIGH) { customMode = (customMode + 1) % 3; delay(200); }
  if (progNow == LOW && progWas == HIGH && customMode == 2) { isProgramming = true; keyToProgram = -1; delay(200); }
  
  modeWas = modeNow;
  progWas = progNow;

  // --- MAIN BRAILLE LOGIC ---
  uint8_t bp = 0;
  for (int i = 0; i < 6; i++) if (digitalRead(braillePins[i]) == LOW) bp |= (1 << i);
  
  if (bp) { 
    braillePattern |= bp; 
    brailleActive = true; 
  } 
  else if (brailleActive) {
    // Special Indicators
    if (braillePattern == 0b100000) {      // Dot 6: Capital Next
      nextIsCap = true;
    } 
    else if (braillePattern == 0b111100) { // Dots 3,4,5,6: Number Next
      nextIsNum = true;
    }
    else {
      char c = getBraille(braillePattern);
      if (c) {
        if (nextIsNum) { c = letterToNumber(c); nextIsNum = false; }
        if (nextIsCap) { if(c >= 'a' && c <= 'z') c -= 32; nextIsCap = false; }

        if (isProgramming && keyToProgram != -1) {
          programmedChars[keyToProgram] = c;
          EEPROM.write(keyToProgram, (uint8_t)c);
          EEPROM.commit();
          isProgramming = false; keyToProgram = -1;
        } else { 
          Keyboard.write(c); 
        }
      }
    }
    braillePattern = 0; brailleActive = false;
  }

  // --- SECONDARY NUMPAD LOGIC (Direct) ---
  uint8_t np = 0;
  for (int i = 0; i < 6; i++) if (digitalRead(numPadPins[i]) == LOW) np |= (1 << i);
  if (np) { numPattern |= np; numActive = true; } 
  else if (numActive) {
    char l = getBraille(numPattern);
    if (l) Keyboard.write(letterToNumber(l));
    numPattern = 0; numActive = false;
  }

  // --- CUSTOM KEYS ---
  for (int i = 0; i < 6; i++) {
    bool isPressed = (digitalRead(customKeys[i]) == LOW);
    if (isPressed && !processed[i]) {
      processed[i] = true; 
      if (isProgramming && i >= 3) { keyToProgram = i - 3; } 
      else if (customMode == 0) { // MEDIA
        if (i == 0) sendMedia(0xB6); if (i == 1) sendMedia(0xCD);
        if (i == 2) sendMedia(0xB5); if (i == 3) sendMedia(0xE2);
        if (i == 4) sendMedia(0xE9); if (i == 5) sendMedia(0xEA);
      } 
      else if (customMode == 1) { // MODIFIERS (Sticky)
        if (i == 0) Keyboard.press(KEY_LEFT_GUI);
        if (i == 1) Keyboard.press(KEY_LEFT_ALT);
        if (i == 2) Keyboard.press(KEY_LEFT_CTRL);
        if (i == 3) sendShortcut('z'); 
        if (i == 4) sendShortcut('y'); 
        if (i == 5) sendShortcut('s'); 
      } 
      else if (customMode == 2) { // PROG/MACRO
        if (i == 0) sendShortcut('c'); 
        if (i == 1) sendShortcut('x'); 
        if (i == 2) sendShortcut('v'); 
        if (i >= 3 && programmedChars[i-3] != ' ') Keyboard.write(programmedChars[i-3]);
      }
    } 
    if (!isPressed) {
      if (processed[i] && customMode == 1 && i < 3) { Keyboard.releaseAll(); }
      processed[i] = false; 
    }
  }

  // --- SPECIAL KEYS ---
  int pins[] = {SPACE_PIN, ENTER_PIN, BACKSPACE_PIN, DELETE_PIN, CAPSLOCK_PIN};
  uint8_t keys[] = {' ', KEY_RETURN, KEY_BACKSPACE, KEY_DELETE, KEY_CAPS_LOCK};
  for(int i=0; i<5; i++) {
    bool p = (digitalRead(pins[i]) == LOW);
    if (p && !specProcessed[i]) { Keyboard.write(keys[i]); specProcessed[i] = true; }
    if (!p) specProcessed[i] = false;
  }

  delay(10); 
}