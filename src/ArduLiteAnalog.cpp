#include "ArduLiteAnalog.h"

Analog::Analog(uint8_t pin) {
    _pin = pin;  // Simpan pin yang digunakan

    ADMUX = 0;
    ADCSRA = 0;

    #if defined(REFS1) && defined(REFS0)
        ADMUX |= _BV(REFS0);
    #else
        ADMUX |= (1 << REFS0);
    #endif

    #if defined(ADEN)
        ADCSRA |= _BV(ADEN);
    #else
        ADCSRA |= (1 << ADEN);
    #endif

    setPrescaler(64); // Default prescaler 64

    // Setup pin yang digunakan
    uint8_t channel = _pin;

    #if defined(__AVR_ATmega2560__)
        if (channel >= 54) channel -= 54;
    #endif

    #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || \
        defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
        if (channel >= 14) channel -= 14;
    #endif

    #if defined(__AVR_ATmega8__)
        if (channel > 5) channel = 5;
    #endif

    #if defined(MUX3)
        ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    #else
        ADMUX = (ADMUX & 0xE0) | (channel & 0x07);
    #endif
}

uint16_t Analog::read() {
    #if defined(ADSC)
        ADCSRA |= _BV(ADSC);
    #else
        ADCSRA |= (1 << ADSC);
    #endif

    while (ADCSRA & _BV(ADSC));
    return ADC;
}

void Analog::setPrescaler(uint8_t factor) {
    ADCSRA &= ~(_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0)); // Reset prescaler

    switch (factor) {
        case 2:  ADCSRA |= _BV(ADPS0); break;
        case 4:  ADCSRA |= _BV(ADPS1); break;
        case 8:  ADCSRA |= _BV(ADPS1) | _BV(ADPS0); break;
        case 16: ADCSRA |= _BV(ADPS2); break;
        case 32: ADCSRA |= _BV(ADPS2) | _BV(ADPS0); break;
        case 64: ADCSRA |= _BV(ADPS2) | _BV(ADPS1); break;
        case 128: ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); break;
        default: ADCSRA |= _BV(ADPS2) | _BV(ADPS1); break; // Default ke 64 jika tidak valid
    }
}
