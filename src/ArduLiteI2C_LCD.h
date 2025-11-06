#ifndef ARDULITE_I2C_LCD_H
#define ARDULITE_I2C_LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include "ArduLiteI2C.h"

// Default I2C Address untuk LCD
#define LCD_I2C_ADDR 0x27

// Pin mapping untuk PCF8574
#define LCD_RS_PIN 0
#define LCD_RW_PIN 1
#define LCD_EN_PIN 2
#define LCD_BL_PIN 3
#define LCD_D4_PIN 4
#define LCD_D5_PIN 5
#define LCD_D6_PIN 6
#define LCD_D7_PIN 7

class I2C_LCD {
  private:
    uint8_t _addr;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _backlight;

    void send_nibble(uint8_t data);
    void send_byte(uint8_t data, uint8_t mode);
    uint8_t get_row_offset(uint8_t row);

  public:
    // Constructor dengan alamat default
    I2C_LCD(uint8_t addr = LCD_I2C_ADDR);
    
    // Inisialisasi LCD dengan ukuran tertentu
    void begin(uint8_t cols = 16, uint8_t rows = 2);
    
    // Fungsi dasar kontrol LCD
    void command(uint8_t cmd);
    void write(char data);
    void clear();
    void home();
    void setCursor(uint8_t col, uint8_t row);
    
    // Fungsi output teks
    void print(const char* str);
    void print(int num);
    void print(float num, uint8_t decimals = 2);
    void print(double num, uint8_t decimals = 2);
    
    // Fungsi advanced
    void createChar(uint8_t location, uint8_t charmap[]);
    void setBacklight(bool on);
    void cursor();
    void noCursor();
    void blink();
    void noBlink();
    void display();
    void noDisplay();
    
    // Getter functions
    uint8_t getCols();
    uint8_t getRows();
    uint8_t getAddress();
};

// =============================================================================
// IMPLEMENTASI METHOD I2C_LCD
// =============================================================================

I2C_LCD::I2C_LCD(uint8_t addr) : _addr(addr), _cols(16), _rows(2), _backlight(0x08) {}

void I2C_LCD::send_nibble(uint8_t data) {
  uint8_t nibble = data | _backlight | (1 << LCD_EN_PIN);
  i2c.write(_addr, 0x00, &nibble, 1);
  _delay_ms(1);
  nibble &= ~(1 << LCD_EN_PIN);
  i2c.write(_addr, 0x00, &nibble, 1);
  _delay_ms(1);
}

void I2C_LCD::send_byte(uint8_t data, uint8_t mode) {
  uint8_t high_nib = mode | _backlight | (data & 0xF0);
  uint8_t low_nib = mode | _backlight | ((data << 4) & 0xF0);
  send_nibble(high_nib);
  send_nibble(low_nib);
}

uint8_t I2C_LCD::get_row_offset(uint8_t row) {
  switch (_rows) {
    case 1:
      return 0x00;
    case 2:
      return (row == 0) ? 0x00 : 0x40;
    case 4:
      switch (row) {
        case 0: return 0x00;
        case 1: return 0x40;
        case 2: return 0x00 + _cols;
        case 3: return 0x40 + _cols;
        default: return 0x00;
      }
    default:
      return 0x00;
  }
}

void I2C_LCD::begin(uint8_t cols, uint8_t rows) {
i2c.begin();
  _cols = cols;
  _rows = rows;

  _delay_ms(50);

  // Inisialisasi 4-bit mode
  send_nibble(0x30);
  _delay_ms(5);
  send_nibble(0x30);
  _delay_ms(1);
  send_nibble(0x30);
  _delay_ms(1);
  send_nibble(0x20); // Function set: 4-bit mode
  _delay_ms(1);

  // Konfigurasi LCD berdasarkan jumlah baris
  uint8_t function_set = 0x28; // 4-bit, 2 lines, 5x8 dots
  if (_rows == 1) {
    function_set = 0x20; // 4-bit, 1 line, 5x8 dots
  }

  command(function_set);
  command(0x0C); // Display ON, Cursor OFF, Blink OFF
  command(0x06); // Entry mode: increment cursor, no shift
  clear();
}

void I2C_LCD::command(uint8_t cmd) {
  send_byte(cmd, 0);
  if (cmd == 0x01 || cmd == 0x02) {
    _delay_ms(2);
  }
}

void I2C_LCD::write(char data) {
  send_byte(data, (1 << LCD_RS_PIN));
}

void I2C_LCD::clear() {
  command(0x01);
  _delay_ms(2);
}

void I2C_LCD::home() {
  command(0x02);
  _delay_ms(2);
}

void I2C_LCD::setCursor(uint8_t col, uint8_t row) {
  if (col >= _cols) col = _cols - 1;
  if (row >= _rows) row = _rows - 1;
  command(0x80 | (col + get_row_offset(row)));
}

void I2C_LCD::print(const char* str) {
  while (*str) write(*str++);
}

void I2C_LCD::print(int num) {
  char buffer[10];
  itoa(num, buffer, 10);
  print(buffer);
}

void I2C_LCD::print(float num, uint8_t decimals) {
  char buffer[20];
  dtostrf(num, 0, decimals, buffer);
  print(buffer);
}

void I2C_LCD::print(double num, uint8_t decimals) {
  char buffer[20];
  dtostrf(num, 0, decimals, buffer);
  print(buffer);
}

void I2C_LCD::createChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7;
  command(0x40 | (location << 3));
  for (uint8_t i = 0; i < 8; i++) {
    write(charmap[i]);
  }
}

void I2C_LCD::setBacklight(bool on) {
  _backlight = on ? (1 << LCD_BL_PIN) : 0x00;
  uint8_t temp = 0;
  i2c.write(_addr, 0x00, &temp, 1);
}

void I2C_LCD::cursor() {
  command(0x0E); // Display ON, Cursor ON
}

void I2C_LCD::noCursor() {
  command(0x0C); // Display ON, Cursor OFF
}

void I2C_LCD::blink() {
  command(0x0D); // Display ON, Blink ON
}

void I2C_LCD::noBlink() {
  command(0x0C); // Display ON, Blink OFF
}

void I2C_LCD::display() {
  command(0x0C); // Display ON
}

void I2C_LCD::noDisplay() {
  command(0x08); // Display OFF
}

uint8_t I2C_LCD::getCols() {
  return _cols;
}

uint8_t I2C_LCD::getRows() {
  return _rows;
}

uint8_t I2C_LCD::getAddress() {
  return _addr;
}

#endif // ARDULITE_I2C_LCD_H