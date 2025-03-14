#ifndef ARDULITE_PWM_H
#define ARDULITE_PWM_H

#include <avr/io.h>

class PWM {
private:
    volatile uint8_t* tccrA;
    volatile uint8_t* tccrB;
    volatile uint8_t* ocr;
    volatile uint16_t* ocr16;
    volatile uint8_t* ddr;
    volatile uint8_t* port;
    uint8_t bit;
    uint8_t com;
    uint8_t wgmA;
    uint8_t wgmB_tccrA;
    uint8_t wgmB_tccrB;
    uint8_t cs;
    bool initialized;

public:
    PWM(uint8_t pin);
    void write(uint8_t value);
};

#endif