#include "ArduLitePWM.h"

PWM::PWM(uint8_t pin) {
    tccrA = nullptr;
    tccrB = nullptr;
    ocr = nullptr;
    ocr16 = nullptr;
    ddr = nullptr;
    port = nullptr;
    initialized = false;

    // Konfigurasi untuk semua chip AVR
    #if defined(__AVR_ATmega8__)
    // ATmega8: Pin 9 (PB1/OC1A), 10 (PB2/OC1B), 11 (PB3/OC2)
    switch (pin) {
        case 9:  // Timer1A
            ocr16 = &OCR1A; tccrA = &TCCR1A; tccrB = &TCCR1B; 
            ddr = &DDRB; port = &PORTB; bit = PB1;
            com = COM1A1; wgmA = WGM10; wgmB_tccrB = WGM12; cs = CS11;
            break;
        case 10: // Timer1B
            ocr16 = &OCR1B; tccrA = &TCCR1A; tccrB = &TCCR1B;
            ddr = &DDRB; port = &PORTB; bit = PB2;
            com = COM1B1; wgmA = WGM10; wgmB_tccrB = WGM12; cs = CS11;
            break;
        case 11: // Timer2
            ocr = &OCR2; tccrA = &TCCR2; tccrB = &TCCR2;
            ddr = &DDRB; port = &PORTB; bit = PB3;
            com = COM21; wgmA = WGM20; wgmB_tccrA = WGM21; cs = CS22;
            break;
        default: return;
    }

    #elif defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || \
          defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || \
          defined(__AVR_ATmega328P__)
    // ATmega48/88/168/328: Pin 3,5,6,9,10,11
    switch (pin) {
        case 3:  // Timer2B
            ocr = &OCR2B; tccrA = &TCCR2A; tccrB = &TCCR2B;
            ddr = &DDRD; port = &PORTD; bit = PD3;
            com = COM2B1; wgmA = WGM20; wgmB_tccrA = WGM21; cs = CS22;
            break;
        case 5:  // Timer0B
            ocr = &OCR0B; tccrA = &TCCR0A; tccrB = &TCCR0B;
            ddr = &DDRD; port = &PORTD; bit = PD5;
            com = COM0B1; wgmA = WGM00; wgmB_tccrA = WGM01; cs = CS01;
            break;
        case 6:  // Timer0A
            ocr = &OCR0A; tccrA = &TCCR0A; tccrB = &TCCR0B;
            ddr = &DDRD; port = &PORTD; bit = PD6;
            com = COM0A1; wgmA = WGM00; wgmB_tccrA = WGM01; cs = CS01;
            break;
        case 9:  // Timer1A
            ocr16 = &OCR1A; tccrA = &TCCR1A; tccrB = &TCCR1B;
            ddr = &DDRB; port = &PORTB; bit = PB1;
            com = COM1A1; wgmA = WGM10; wgmB_tccrB = WGM12; cs = CS11;
            break;
        case 10: // Timer1B
            ocr16 = &OCR1B; tccrA = &TCCR1A; tccrB = &TCCR1B;
            ddr = &DDRB; port = &PORTB; bit = PB2;
            com = COM1B1; wgmA = WGM10; wgmB_tccrB = WGM12; cs = CS11;
            break;
        case 11: // Timer2A
            ocr = &OCR2A; tccrA = &TCCR2A; tccrB = &TCCR2B;
            ddr = &DDRB; port = &PORTB; bit = PB3;
            com = COM2A1; wgmA = WGM20; wgmB_tccrA = WGM21; cs = CS22;
            break;
        default: return;
    }

    #elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
    // ATmega2560/1280: Pin 2-13,44-46
    switch (pin) {
        case 2:  // Timer3B
            ocr16 = &OCR3B; tccrA = &TCCR3A; tccrB = &TCCR3B;
            ddr = &DDRE; port = &PORTE; bit = PE4;
            com = COM3B1; wgmA = WGM30; wgmB_tccrB = WGM32; cs = CS31;
            break;
        case 3:  // Timer3C
            ocr16 = &OCR3C; tccrA = &TCCR3A; tccrB = &TCCR3B;
            ddr = &DDRE; port = &PORTE; bit = PE5;
            com = COM3C1; wgmA = WGM30; wgmB_tccrB = WGM32; cs = CS31;
            break;
        case 5:  // Timer3A
            ocr16 = &OCR3A; tccrA = &TCCR3A; tccrB = &TCCR3B;
            ddr = &DDRE; port = &PORTE; bit = PE3;
            com = COM3A1; wgmA = WGM30; wgmB_tccrB = WGM32; cs = CS31;
            break;
        case 6:  // Timer4A
            ocr16 = &OCR4A; tccrA = &TCCR4A; tccrB = &TCCR4B;
            ddr = &DDRH; port = &PORTH; bit = PH3;
            com = COM4A1; wgmA = WGM40; wgmB_tccrB = WGM42; cs = CS41;
            break;
        case 7:  // Timer4B
            ocr16 = &OCR4B; tccrA = &TCCR4A; tccrB = &TCCR4B;
            ddr = &DDRH; port = &PORTH; bit = PH4;
            com = COM4B1; wgmA = WGM40; wgmB_tccrB = WGM42; cs = CS41;
            break;
        case 8:  // Timer4C
            ocr16 = &OCR4C; tccrA = &TCCR4A; tccrB = &TCCR4B;
            ddr = &DDRH; port = &PORTH; bit = PH5;
            com = COM4C1; wgmA = WGM40; wgmB_tccrB = WGM42; cs = CS41;
            break;
        case 9:  // Timer2A
            ocr = &OCR2A; tccrA = &TCCR2A; tccrB = &TCCR2B;
            ddr = &DDRB; port = &PORTB; bit = PB4;
            com = COM2A1; wgmA = WGM20; wgmB_tccrA = WGM21; cs = CS22;
            break;
        case 10: // Timer2B
            ocr = &OCR2B; tccrA = &TCCR2A; tccrB = &TCCR2B;
            ddr = &DDRB; port = &PORTB; bit = PB5;
            com = COM2B1; wgmA = WGM20; wgmB_tccrA = WGM21; cs = CS22;
            break;
        case 11: // Timer1A
            ocr16 = &OCR1A; tccrA = &TCCR1A; tccrB = &TCCR1B;
            ddr = &DDRB; port = &PORTB; bit = PB5;
            com = COM1A1; wgmA = WGM10; wgmB_tccrB = WGM12; cs = CS11;
            break;
        case 12: // Timer1B
            ocr16 = &OCR1B; tccrA = &TCCR1A; tccrB = &TCCR1B;
            ddr = &DDRB; port = &PORTB; bit = PB6;
            com = COM1B1; wgmA = WGM10; wgmB_tccrB = WGM12; cs = CS11;
            break;
        case 13: // Timer0A
            ocr = &OCR0A; tccrA = &TCCR0A; tccrB = &TCCR0B;
            ddr = &DDRB; port = &PORTB; bit = PB7;
            com = COM0A1; wgmA = WGM00; wgmB_tccrA = WGM01; cs = CS01;
            break;
        default: return;
    }
    #endif

    if (!ddr) return;
    *ddr |= (1 << bit);

    if (ocr16) { // 16-bit Timer
        *tccrA |= (1 << com) | (1 << wgmA);
        *tccrB |= (1 << wgmB_tccrB) | (1 << cs);
    } else {     // 8-bit Timer
        *tccrA |= (1 << com) | (1 << wgmA) | (1 << wgmB_tccrA);
        *tccrB |= (1 << cs);
    }

    initialized = true;
}

void PWM::write(uint8_t value) {
    if (!initialized) return;

    if (value == 0) {
        // Matikan PWM dan set pin LOW
        if (ocr16) *tccrA &= ~((1 << (com - 1)) | (1 << com)); // COM1A1/COM1B1
        else *tccrA &= ~((1 << com) | (1 << (com - 1)));
        *port &= ~(1 << bit);
    } else if (value == 255) {
        // Matikan PWM dan set pin HIGH
        if (ocr16) *tccrA &= ~((1 << (com - 1)) | (1 << com));
        else *tccrA &= ~((1 << com) | (1 << (com - 1)));
        *port |= (1 << bit);
    } else {
        // Aktifkan PWM
        if (ocr16) *ocr16 = value;
        else if (ocr) *ocr = value;
        *tccrA |= (1 << com) | (1 << (com - 1));
    }
}