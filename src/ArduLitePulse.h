// ArduLitePulse.h
#ifndef ARDULITE_PULSE_H
#define ARDULITE_PULSE_H

#include <avr/io.h>
#include "ArduLiteTime.h"  // Include Time header

// Deklarasi global time instance
extern Time time;

#define IN 0
#define OUT 1

class Pulse {
  private:
    uint8_t _pin;
    uint8_t _mode;
    volatile uint8_t* _portReg;
    volatile uint8_t* _ddrReg;
    uint8_t _mask;
    unsigned long _timeout;
    
    void setupPin() {
      if (_pin <= 7) {
        _portReg = &PIND;
        _ddrReg = &DDRD;
        _mask = (1 << _pin);
      } else if (_pin <= 13) {
        _portReg = &PINB;
        _ddrReg = &DDRB;
        _mask = (1 << (_pin - 8));
      } else {
        _portReg = &PINC;
        _ddrReg = &DDRC;
        _mask = (1 << (_pin - 14));
      }
      
      if (_mode == IN) {
        *_ddrReg &= ~_mask; // Set as input
      } else {
        *_ddrReg |= _mask; // Set as output
      }
    }
    
    bool isHigh() {
      return (*_portReg & _mask) != 0;
    }
    
    bool isLow() {
      return (*_portReg & _mask) == 0;
    }
    
  public:
    Pulse(uint8_t pin, uint8_t mode = IN, unsigned long timeoutUs = 30000UL) {
      _pin = pin;
      _mode = mode;
      _timeout = timeoutUs;
      setupPin();
    }
    
    // Baca durasi pulse HIGH (dalam mikrodetik)
    unsigned long read() {
      unsigned long maxTime = time.micros() + _timeout;
      
      // Tunggu LOW dulu
      while (isHigh()) {
        if (time.micros() >= maxTime) return 0;
      }
      
      // Tunggu HIGH
      while (isLow()) {
        if (time.micros() >= maxTime) return 0;
      }
      
      // Ukur durasi HIGH
      unsigned long start = time.micros();
      while (isHigh()) {
        if (time.micros() >= maxTime) return 0;
      }
      
      return time.micros() - start;
    }
    
    // Baca durasi pulse LOW
    unsigned long readLow() {
      unsigned long maxTime = time.micros() + _timeout;
      
      // Tunggu HIGH dulu
      while (isLow()) {
        if (time.micros() >= maxTime) return 0;
      }
      
      // Tunggu LOW
      while (isHigh()) {
        if (time.micros() >= maxTime) return 0;
      }
      
      // Ukur durasi LOW
      unsigned long start = time.micros();
      while (isLow()) {
        if (time.micros() >= maxTime) return 0;
      }
      
      return time.micros() - start;
    }
    
    // Set timeout baru
    void setTimeout(unsigned long timeoutUs) {
      _timeout = timeoutUs;
    }
};

#endif