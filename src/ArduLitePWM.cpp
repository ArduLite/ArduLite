#include "ArduLitePWM.h"

PWM::PWM(uint8_t pin) {
    tccrA = nullptr;
    tccrB = nullptr;
    ocr = nullptr;
    ocr16 = nullptr;
    ddr = nullptr;
    initialized = false;

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
    switch (pin) {
        case 3:  // Timer2B
            ocr = &OCR2B; tccrA = &TCCR2A; tccrB = &TCCR2B; ddr = &DDRD; bit = PD3;
            com = COM2B1; wgmA = WGM20; wgmB_tccrA = WGM21; cs = CS22;
            wgmB_tccrB = 0;
            break;
        case 5:  // Timer0B
            ocr = &OCR0B; tccrA = &TCCR0A; tccrB = &TCCR0B; ddr = &DDRD; bit = PD5;
            com = COM0B1; wgmA = WGM00; wgmB_tccrA = WGM01; cs = CS01;
            wgmB_tccrB = 0;
            break;
        case 6:  // Timer0A
            ocr = &OCR0A; tccrA = &TCCR0A; tccrB = &TCCR0B; ddr = &DDRD; bit = PD6;
            com = COM0A1; wgmA = WGM00; wgmB_tccrA = WGM01; cs = CS01;
            wgmB_tccrB = 0;
            break;
        case 9:  // Timer1A
            ocr16 = &OCR1A; tccrA = &TCCR1A; tccrB = &TCCR1B; ddr = &DDRB; bit = PB1;
            com = COM1A1; wgmA = WGM10; wgmB_tccrA = 0;
            wgmB_tccrB = WGM12; // Mode 5 (Fast PWM 8-bit)
            cs = CS11; // Prescaler 8
            break;
        case 10: // Timer1B
            ocr16 = &OCR1B; tccrA = &TCCR1A; tccrB = &TCCR1B; ddr = &DDRB; bit = PB2;
            com = COM1B1; wgmA = WGM10; wgmB_tccrA = 0;
            wgmB_tccrB = WGM12;
            cs = CS11;
            break;
        case 11: // Timer2A
            ocr = &OCR2A; tccrA = &TCCR2A; tccrB = &TCCR2B; ddr = &DDRB; bit = PB3;
            com = COM2A1; wgmA = WGM20; wgmB_tccrA = WGM21; cs = CS22;
            wgmB_tccrB = 0;
            break;
        default:
            return;
    }

    *ddr |= (1 << bit); // Set pin sebagai output

    if (ocr16) { // Konfigurasi Timer1 (16-bit)
        *tccrA |= (1 << com) | (1 << wgmA);
        *tccrB |= (1 << wgmB_tccrB) | (1 << cs);
    } else { // Konfigurasi Timer0/Timer2 (8-bit)
        *tccrA |= (1 << com) | (1 << wgmA) | (1 << wgmB_tccrA);
        *tccrB |= (1 << cs);
    }

    initialized = true;
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