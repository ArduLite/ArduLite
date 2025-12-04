#ifndef ARDULITE_SERVO_H
#define ARDULITE_SERVO_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ArduLiteTime.h"

class ArduLiteServo {
  private:
    uint8_t _pin;
    uint16_t _pulseMin;
    uint16_t _pulseMax;
    uint16_t _angle;
    bool _attached;
    uint8_t _timerChannel;
    
    void setupTimer1() {
        TCCR1A = 0;
        TCCR1B = 0;
        
        // Fast PWM mode, TOP = ICR1
        TCCR1A |= (1 << WGM11);
        TCCR1B |= (1 << WGM12) | (1 << WGM13);
        
        // Set prescaler to 8 (2MHz clock, 0.5us resolution)
        TCCR1B |= (1 << CS11);
        
        // Set period to 20ms (50Hz)
        ICR1 = 39999;
        
        // Enable output compare based on channel
        if (_timerChannel == 1) {
            TCCR1A |= (1 << COM1A1);
        } else {
            TCCR1A |= (1 << COM1B1);
        }
    }
    
    void setPWM(uint16_t value) {
        if (value > ICR1) value = ICR1;
        if (_timerChannel == 1) {
            OCR1A = value;
        } else {
            OCR1B = value;
        }
    }
    
    void setPinOutput(uint8_t pin) {
        volatile uint8_t* ddr;
        
        if (pin <= 7) ddr = &DDRD;
        else if (pin <= 13) ddr = &DDRB;
        else ddr = &DDRC;
        
        uint8_t bit = (pin <= 7 ? pin : (pin <= 13 ? pin - 8 : pin - 14));
        *ddr |= (1 << bit);
    }
    
    uint8_t getTimerChannel(uint8_t pin) {
        switch(pin) {
            case 9:  return 1;  // OCR1A
            case 10: return 2;  // OCR1B
            default: return 0;
        }
    }

  public:
    ArduLiteServo() : _pin(255), _pulseMin(544), _pulseMax(2400), _angle(0), _attached(false), _timerChannel(0) {}
    
    bool attach(uint8_t pin, uint16_t min = 544, uint16_t max = 2400) {
        if (pin != 9 && pin != 10) return false;
        
        _pin = pin;
        _pulseMin = min;
        _pulseMax = max;
        _timerChannel = getTimerChannel(pin);
        
        if (_timerChannel == 0) return false;
        
        setPinOutput(_pin);
        setupTimer1();
        _attached = true;
        
        write(90);
        return true;
    }
    
    void detach() {
        if (!_attached) return;
        
        if (_timerChannel == 1) {
            TCCR1A &= ~(1 << COM1A1);
        } else {
            TCCR1A &= ~(1 << COM1B1);
        }
        
        _attached = false;
    }
    
    void write(uint8_t angle) {
        if (!_attached) return;
        
        _angle = (angle > 180) ? 180 : angle;
        
        // Calculate pulse width in microseconds
        uint32_t pulseWidth = _pulseMin + ((uint32_t)(_angle) * (_pulseMax - _pulseMin)) / 180;
        
        // Convert to timer ticks (0.5us per tick)
        uint16_t ticks = pulseWidth * 2;
        setPWM(ticks);
    }
    
    void writeMicroseconds(uint16_t us) {
        if (!_attached) return;
        
        if (us < _pulseMin) us = _pulseMin;
        if (us > _pulseMax) us = _pulseMax;
        
        _angle = ((uint32_t)(us - _pulseMin) * 180) / (_pulseMax - _pulseMin);
        uint16_t ticks = us * 2;
        setPWM(ticks);
    }
    
    uint8_t read() {
        return _angle;
    }
    
    uint16_t readMicroseconds() {
        return _pulseMin + ((uint32_t)(_angle) * (_pulseMax - _pulseMin)) / 180;
    }
    
    bool attached() {
        return _attached;
    }
};

#endif