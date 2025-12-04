/*
  Created by: Ajang Rahmat
  Date      : 15-11-2025
  Keypad Circuit:
    Row Pins  -> Arduino Pins: 9, 8, 7, 6
    Column Pins -> Arduino Pins: 5, 4, 3
    4x3 Keypad Layout:
        1  2  3
        4  5  6
        7  8  9
        *  0  #
*/

#include <ArduLite.h>
#include <ArduLiteUART.h>
#include <ArduLiteKeypad.h>

uint8_t rowPins[] = {9, 8, 7, 6};
uint8_t colPins[] = {5, 4, 3};

const uint8_t NUM_ROWS = 4;
const uint8_t NUM_COLS = 3;

const char KEYPAD_MAP[NUM_ROWS][NUM_COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad keypad(NUM_ROWS, NUM_COLS, rowPins, colPins);

int main() {
  uart.begin(9600);
  keypad.begin();
  keypad.setFlexibleKeymap(KEYPAD_MAP);
  uart.sendLine("Keypad Ready!");

  while (true) {
    char key = keypad.getKey();
    if (key) {
      uart.send("Key: ");
      uart.sendLine(key);
    }
  }
}
