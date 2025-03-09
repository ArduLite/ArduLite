#ifndef ARDULITE_PWM_H
#define ARDULITE_PWM_H

#include <avr/io.h>

class PWM {
private:
    volatile uint8_t *tccrA;
    volatile uint8_t *tccrB;
    volatile uint8_t *ocr;
    volatile uint16_t *ocr16;
    volatile uint8_t *ddr; // Tambahkan register DDR
    uint8_t bit;  // Posisi bit pada DDR

    uint8_t com, wgmA, wgmB, cs;
    bool initialized;

public:
    PWM(uint8_t pin);
    void write(uint8_t value);
};

#endif // ARDULITE_PWM_H
