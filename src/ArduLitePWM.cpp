#include "ArduLitePWM.h"

PWM::PWM(uint8_t pin) {
    tccrA = nullptr;
    tccrB = nullptr;
    ocr = nullptr;
    ocr16 = nullptr;
    ddr = nullptr;
    initialized = false;

    tccrA = nullptr;
    tccrB = nullptr;
    ocr = nullptr;
    ocr16 = nullptr;
    initialized = true;

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)

    switch (pin) {
        case 3:  // Timer2B (8-bit PWM)
            ocr = &OCR2B; tccrA = &TCCR2A; tccrB = &TCCR2B; ddr = &DDRD; bit = PD3;
            com = COM2B1; wgmA = WGM20; wgmB = WGM21; cs = CS22;
            break;
        case 5:  // Timer0B (8-bit PWM)
            ocr = &OCR0B; tccrA = &TCCR0A; tccrB = &TCCR0B; ddr = &DDRD; bit = PD5;
            com = COM0B1; wgmA = WGM00; wgmB = WGM01; cs = CS01;
            break;
        case 6:  // Timer0A (8-bit PWM)
            ocr = &OCR0A; tccrA = &TCCR0A; tccrB = &TCCR0B; ddr = &DDRD; bit = PD6;
            com = COM0A1; wgmA = WGM00; wgmB = WGM01; cs = CS01;
            break;
        case 9:  // Timer1A (16-bit PWM)
            ocr16 = &OCR1A; tccrA = &TCCR1A; tccrB = &TCCR1B; ddr = &DDRB; bit = PB1;
            com = COM1A1; wgmA = WGM10; wgmB = WGM11; cs = CS11;
            break;
        case 10: // Timer1B (16-bit PWM)
            ocr16 = &OCR1B; tccrA = &TCCR1A; tccrB = &TCCR1B; ddr = &DDRB; bit = PB2;
            com = COM1B1; wgmA = WGM10; wgmB = WGM11; cs = CS11;
            break;
        case 11: // Timer2A (8-bit PWM)
            ocr = &OCR2A; tccrA = &TCCR2A; tccrB = &TCCR2B; ddr = &DDRB; bit = PB3;
            com = COM2A1; wgmA = WGM20; wgmB = WGM21; cs = CS22;
            break;
        default:
            initialized = false;
            return;
    }

    if (ddr) *ddr |= (1 << bit);

    if (tccrA) *tccrA |= (1 << com) | (1 << wgmA) | (1 << wgmB);
    if (tccrB) *tccrB |= (1 << cs);

#endif
}

void PWM::write(uint8_t value) {
    if (!initialized) return;

    if (ocr16) {
        *ocr16 = value;
    } else if (ocr) {
        *ocr = value;
    }
}
