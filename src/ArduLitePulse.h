// ArduLitePulse.h
#ifndef ARDULITE_PULSE_H
#define ARDULITE_PULSE_H

#include <avr/io.h>

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
    
    void startTimer() {
      TCCR1A = 0;
      TCCR1B = (1 << CS11); // Prescaler 8
      TCNT1 = 0;
    }
    
    unsigned long getTicks() {
      return TCNT1;
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
      unsigned long maxTicks = _timeout * 2;
      
      // Tunggu LOW dulu
      startTimer();
      while (isHigh()) {
        if (getTicks() >= maxTicks) return 0;
      }
      
      // Tunggu HIGH
      startTimer();
      while (isLow()) {
        if (getTicks() >= maxTicks) return 0;
      }
      
      // Ukur durasi HIGH
      startTimer();
      while (isHigh()) {
        if (getTicks() >= maxTicks) return 0;
      }
      
      return getTicks() / 2;
    }
    
    // Baca durasi pulse LOW
    unsigned long readLow() {
      unsigned long maxTicks = _timeout * 2;
      
      // Tunggu HIGH dulu
      startTimer();
      while (isLow()) {
        if (getTicks() >= maxTicks) return 0;
      }
      
      // Tunggu LOW
      startTimer();
      while (isHigh()) {
        if (getTicks() >= maxTicks) return 0;
      }
      
      // Ukur durasi LOW
      startTimer();
      while (isLow()) {
        if (getTicks() >= maxTicks) return 0;
      }
      
      return getTicks() / 2;
    }
    
    // Set timeout baru
    void setTimeout(unsigned long timeoutUs) {
      _timeout = timeoutUs;
    }
};

#endif