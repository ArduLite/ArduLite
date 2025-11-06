#ifndef ARDULITE_LCD_H
#define ARDULITE_LCD_H

#include <avr/io.h>
#include <util/delay.h>

class LCD {
  private:
    uint8_t _rs, _en, _d4, _d5, _d6, _d7;
    uint8_t _cols, _rows;
    bool _useBacklight = false;
    bool _usePWM = false;

    uint8_t _blPin;
    volatile uint8_t* _port_bl;
    volatile uint8_t* _ddr_bl;
    uint8_t _mask_bl;

    volatile uint8_t* _port_rs; volatile uint8_t* _ddr_rs; uint8_t _mask_rs;
    volatile uint8_t* _port_en; volatile uint8_t* _ddr_en; uint8_t _mask_en;
    volatile uint8_t* _port_d4; volatile uint8_t* _ddr_d4; uint8_t _mask_d4;
    volatile uint8_t* _port_d5; volatile uint8_t* _ddr_d5; uint8_t _mask_d5;
    volatile uint8_t* _port_d6; volatile uint8_t* _ddr_d6; uint8_t _mask_d6;
    volatile uint8_t* _port_d7; volatile uint8_t* _ddr_d7; uint8_t _mask_d7;

    void mapPin(uint8_t pin, volatile uint8_t** port, volatile uint8_t** ddr, uint8_t* mask) {
      if (pin <= 7) { *port = &PORTD; *ddr = &DDRD; *mask = (1 << pin); }
      else if (pin <= 13) { *port = &PORTB; *ddr = &DDRB; *mask = (1 << (pin - 8)); }
      else { *port = &PORTC; *ddr = &DDRC; *mask = (1 << (pin - 14)); }
    }

    void writeBit(volatile uint8_t* port, uint8_t mask, bool high) {
      if (high) *port |= mask; else *port &= ~mask;
    }

    void pulseEnable() { *_port_en |= _mask_en; _delay_us(1); *_port_en &= ~_mask_en; _delay_us(100); }

    void write4bits(uint8_t value) {
      writeBit(_port_d4, _mask_d4, value & 0x01);
      writeBit(_port_d5, _mask_d5, value & 0x02);
      writeBit(_port_d6, _mask_d6, value & 0x04);
      writeBit(_port_d7, _mask_d7, value & 0x08);
      pulseEnable();
    }

    void send(uint8_t value, bool modeData) {
      writeBit(_port_rs, _mask_rs, modeData);
      write4bits(value >> 4);
      write4bits(value & 0x0F);
    }

    void setupPWM() {
      // Timer1 Fast PWM 8-bit mode, non-inverting, prescaler 64
      TCCR1A |= (1 << COM1B1) | (1 << WGM10);
      TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
      OCR1B = 255;
      _usePWM = true;
    }

  public:
    // --- Konstruktor utama tanpa backlight ---
    LCD(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
      _rs = rs; _en = en; _d4 = d4; _d5 = d5; _d6 = d6; _d7 = d7;

      mapPin(_rs, &_port_rs, &_ddr_rs, &_mask_rs);
      mapPin(_en, &_port_en, &_ddr_en, &_mask_en);
      mapPin(_d4, &_port_d4, &_ddr_d4, &_mask_d4);
      mapPin(_d5, &_port_d5, &_ddr_d5, &_mask_d5);
      mapPin(_d6, &_port_d6, &_ddr_d6, &_mask_d6);
      mapPin(_d7, &_port_d7, &_ddr_d7, &_mask_d7);

      *_ddr_rs |= _mask_rs;
      *_ddr_en |= _mask_en;
      *_ddr_d4 |= _mask_d4;
      *_ddr_d5 |= _mask_d5;
      *_ddr_d6 |= _mask_d6;
      *_ddr_d7 |= _mask_d7;
    }

    // --- Aktifkan kontrol backlight (opsional) ---
    void enableBacklight(uint8_t pin, bool usePWM = false) {
      _useBacklight = true;
      _usePWM = usePWM;
      _blPin = pin;
      mapPin(_blPin, &_port_bl, &_ddr_bl, &_mask_bl);
      *_ddr_bl |= _mask_bl;

      if (_usePWM) {
        setupPWM();
      } else {
        *_port_bl |= _mask_bl; // nyalakan default
      }
    }

    void begin(uint8_t cols, uint8_t rows) {
      _cols = cols; _rows = rows;
      _delay_ms(20);
      send(0x33,0); send(0x32,0);
      command(0x28); command(0x0C); command(0x06);
      clear();
    }

    // --- Fungsi Backlight ---
    void backlightOn() {
      if (_useBacklight && !_usePWM) *_port_bl |= _mask_bl;
    }
    void backlightOff() {
      if (_useBacklight && !_usePWM) *_port_bl &= ~_mask_bl;
    }
    void backlightPWM(uint8_t brightness) {
      if (_useBacklight && _usePWM) OCR1B = brightness;
    }

    // --- Fungsi dasar LCD ---
    void clear() { command(0x01); _delay_ms(2); }
    void home() { command(0x02); _delay_ms(2); }
    void command(uint8_t cmd) { send(cmd,0); _delay_ms(2); }
    void writeChar(char c) { send(c,1); }
    void print(const char* str) { while(*str) writeChar(*str++); }
    void setCursor(uint8_t col, uint8_t row) {
      if(row>=_rows) row=0;
      uint8_t addr = (row==0)? col : (0x40+col);
      command(0x80 | addr);
    }
};

#endif
