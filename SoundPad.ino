/* SoundPad
    eschmitt 2026
*/
#include <Keypad.h>
#include "Keyboard.h"

// Keystrokes defined by ASCII code + 136
// ASCII codes can be found on page 53 of https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

#define KEY_NUM_1 225
#define KEY_NUM_2 226
#define KEY_NUM_3 227
#define KEY_NUM_4 228
#define KEY_NUM_5 229
#define KEY_NUM_6 230
#define KEY_NUM_7 231
#define KEY_NUM_8 232
#define KEY_NUM_9 233
#define KEY_NUM_0 234
#define KEY_G 146
#define KEY_B 141

//define button matrix layout
const byte ROWS = 5; //four rows
const byte COLS = 4; //four columns

//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '6', 'B', 'G'},
  {'2', '7', 'C', 'H'},
  {'3', '8', 'D', 'I'},
  {'4', '9', 'E', 'J'},
  {'5', 'A', 'F', 'K'}
};

//define button matrix connections
byte rowPins[ROWS] = {6, 20, 8, 9, 10}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4, 5}; //connect to the column pinouts of the keypad

char customKey;

const int footPin = 7;
const int pagePin = 16;
const int modPin = 1;
const int page1Led = 15;
const int page2Led = 18;
const int modLed = 19;
int pageButton = 0;

//below are volatile because they are used in ISR
volatile int modButton = 0;
volatile int footClicks = 0;
bool footPress = false;

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  Keyboard.begin();
  pinMode(page1Led, OUTPUT);
  pinMode(page2Led, OUTPUT);
  pinMode(modLed, OUTPUT);
  pinMode(pagePin, INPUT_PULLUP);
  pinMode(modPin, INPUT_PULLUP);
  pinMode(footPin, INPUT_PULLUP);
  digitalWrite(modLed, LOW);
  attachInterrupt(digitalPinToInterrupt(footPin), footChangeISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(modPin), modChangeISR, CHANGE);
}

void loop() {
  checkMods(); //check modifier switches
  customKey = customKeypad.getKey(); //check button matrix for key presses
  if (footPress) {  //when foot switch is pressed, footChangeISR sets footPress flag to true
    switch (modButton) {  //modButton flag is changed in loop(in checkMods()) since it is a latching switch and timing isn't critical
      case 0: // toggle state
        switch (footClicks) {  //footClicks counter increments on state change via foot switch ISR. This is needed since foot switch is momentary
          case 1:
            footPress = false;
            Keyboard.press(KEY_B); //Press 'b'
            digitalWrite(modLed, !digitalRead(modLed));
            Keyboard.release(KEY_B);
            break;
          case 2:
            footPress = false;
            footClicks = 0;
            break;
        }
        break;
      case 1: // PTT state
        Keyboard.press(KEY_B); //Press 'b'
        digitalWrite(modLed, !digitalRead(modLed));
        Keyboard.release(KEY_B);
        footPress = false;
        break;
    }
  }
  if (customKey) {   //run keyMap functions according to page switch position
    switch (pageButton) {
      case 0:
        keyMap1();
        break;
      case 1:
        keyMap2();
        break;
    }
  }
}


void checkMods() {
  //Check page switch, illuminate correct LED and set pageButton value
  if (digitalRead(pagePin) == HIGH) {
    pageButton = 1;
    digitalWrite(page2Led, LOW); // LED anode is connected to arduino, LOW turns LED on
    digitalWrite(page1Led, HIGH);
  }
  else {
    pageButton = 0;
    digitalWrite(page1Led, LOW);
    digitalWrite(page2Led, HIGH);
  }
  //Check mod switch, illuminate LED and set modButton value
  if (digitalRead(modPin) == HIGH) {
    modButton = 0;
  }
  else {
    modButton = 1;
  }
}

void sendCombo(uint8_t b1, uint8_t b2 = 0) {  //this function is called by keyMap function and sends the CTRL+ALT+xx keyboard strokes, where x is a numpad key. Desired keys are provided as arguments b1 and b2.
  Keyboard.press(KEY_RIGHT_CTRL);
  Keyboard.press(KEY_RIGHT_ALT);
  Keyboard.press(b1);
  if (b2 != 0) {
    Keyboard.release(b1);
    Keyboard.press(b2);
  }
  Keyboard.releaseAll();
}

void keyMap1() {  //switch case that determines the correct key sequence to send when a button is pressed and page 1 is selected. 
  switch (customKey) {
    case '1':
      sendCombo(KEY_NUM_1);
      break;
    case '2':
      sendCombo(KEY_NUM_2);
      break;
    case '3':
      sendCombo(KEY_NUM_3);
      break;
    case '4':
      sendCombo(KEY_NUM_4);
      break;
    case '5':
      sendCombo(KEY_NUM_5);
      break;
    case '6':
      sendCombo(KEY_NUM_6);
      break;
    case '7':
      sendCombo(KEY_NUM_7);
      break;
    case '8':
      sendCombo(KEY_NUM_8);
      break;
    case '9':
      sendCombo(KEY_NUM_9);
      break;
    case 'A': //10
      sendCombo(KEY_NUM_1, KEY_NUM_0);
      break;
    case 'B': //11
      sendCombo(KEY_NUM_1, KEY_NUM_1);
      break;
    case 'C': //12
      sendCombo(KEY_NUM_1, KEY_NUM_2);
      break;
    case 'D': //13
      sendCombo(KEY_NUM_1, KEY_NUM_3);
      break;
    case 'E': //14
      sendCombo(KEY_NUM_1, KEY_NUM_4);
      break;
    case 'F': //15
      sendCombo(KEY_NUM_1, KEY_NUM_5);
      break;
    case 'G': //16
      sendCombo(KEY_NUM_1, KEY_NUM_6);
      break;
    case 'H': //17
      sendCombo(KEY_NUM_1, KEY_NUM_7);
      break;
    case 'I': //18
      sendCombo(KEY_NUM_1, KEY_NUM_8);
      break;
    case 'J': //19
      sendCombo(KEY_NUM_1, KEY_NUM_9);
      break;
    case 'K': //20
      sendCombo(KEY_NUM_2, KEY_NUM_0);
      break;
  }
}

void keyMap2() {  //switch case that determines the correct key sequence to send when a button is pressed and page 2 is selected. 
  switch (customKey) {
    case '1': //21
      sendCombo(KEY_NUM_2, KEY_NUM_1);
      break;
    case '2': //22
      sendCombo(KEY_NUM_2, KEY_NUM_2);
      break;
    case '3': //23
      sendCombo(KEY_NUM_2, KEY_NUM_3);
      break;
    case '4': //24
      sendCombo(KEY_NUM_2, KEY_NUM_4);
      break;
    case '5':  //25
      sendCombo(KEY_NUM_2, KEY_NUM_5);
      break;
    case '6': //26
      sendCombo(KEY_NUM_2, KEY_NUM_6);
      break;
    case '7': //27
      sendCombo(KEY_NUM_2, KEY_NUM_7);
      break;
    case '8': //28
      sendCombo(KEY_NUM_2, KEY_NUM_8);
      break;
    case '9': //29
      sendCombo(KEY_NUM_2, KEY_NUM_9);
      break;
    case 'A': //30
      sendCombo(KEY_NUM_3, KEY_NUM_0);
      break;
    case 'B': //31
      sendCombo(KEY_NUM_3, KEY_NUM_1);
      break;
    case 'C': //32
      sendCombo(KEY_NUM_3, KEY_NUM_2);
      break;
    case 'D': //33
      sendCombo(KEY_NUM_3, KEY_NUM_3);
      break;
    case 'E': //34
      sendCombo(KEY_NUM_3, KEY_NUM_4);
      break;
    case 'F': //35
      sendCombo(KEY_NUM_3, KEY_NUM_5);
      break;
    case 'G': //36
      sendCombo(KEY_NUM_3, KEY_NUM_6);
      break;
    case 'H': //37
      sendCombo(KEY_NUM_3, KEY_NUM_7);
      break;
    case 'I': //38
      sendCombo(KEY_NUM_3, KEY_NUM_8);
      break;
    case 'J': //39
      sendCombo(KEY_NUM_3, KEY_NUM_9);
      break;
    case 'K': // Win+Alt+g  - video capture
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_RIGHT_ALT);
      Keyboard.press(KEY_G); // 'g'
      Keyboard.releaseAll();
      break;
  }
}

void modChangeISR() {  // this ISR essentially functions as a reset for sync issues in game. LED is switched off, foot click counter is reset and all pressed keys are released. 
  digitalWrite(modLed, LOW);
  footClicks = 0;
  Keyboard.releaseAll();
}

void footChangeISR() { // this ISR sets the footPress flag and increments the footClicks counter, to be handled next loop. 
  footPress = true;
  footClicks++;
}
