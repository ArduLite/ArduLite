/*
  Created by: Ajang Rahmat
  Date      : 15-11-2025
  Keypad Circuit:
    Row Pins    -> Arduino Pins: 9, 8, 7, 6
    Column Pins -> Arduino Pins: 5, 4, 3, 2
    4x4 Keypad Layout:
        1  2  3  A
        4  5  6  B
        7  8  9  C
        *  0  #  D
*/

#include <ArduLite.h>
#include <ArduLiteUART.h>
#include <ArduLiteKeypad.h>

uint8_t rowPins[] = {9, 8, 7, 6};
uint8_t colPins[] = {5, 4, 3, 2};

const uint8_t NUM_ROWS = 4;
const uint8_t NUM_COLS = 4;

const char KEYPAD_MAP[NUM_ROWS][NUM_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

Keypad keypad(NUM_ROWS, NUM_COLS, rowPins, colPins);

int main() {
  uart.begin(9600);
  keypad.begin();
  keypad.setFlexibleKeymap(KEYPAD_MAP);
  uart.sendLine("4x4 Keypad Ready!");

  while (true) {
    char key = keypad.getKey();
    if (key) {
      uart.send("Key: ");
      uart.sendLine(key);
    }
  }
}
