#include "ArduLiteAnalog.h"

Analog::Analog(uint8_t pin) {
    _pin = pin;  // Simpan pin yang digunakan

    ADMUX = 0;
    ADCSRA = 0;

    // Set referensi tegangan ke AVCC (5V)
    #if defined(REFS1) && defined(REFS0)
        ADMUX |= _BV(REFS0); // REFS0 = 1, REFS1 = 0 (AVCC sebagai referensi)
    #else
        ADMUX |= (1 << REFS0);
    #endif

    // Aktifkan ADC
    #if defined(ADEN)
        ADCSRA |= _BV(ADEN);
    #else
        ADCSRA |= (1 << ADEN);
    #endif

    setPrescaler(64); // Default prescaler 64 (clock ADC = 125kHz @ 8MHz)

    // Konversi pin Arduino ke channel ADC
    uint8_t channel = _pin;

    // ATmega2560: pin analog dimulai dari 54 (A0=54, A1=55, dst)
    #if defined(__AVR_ATmega2560__)
        if (channel >= 54) channel -= 54;
    #endif

    // ATmega328P: pin analog dimulai dari 14 (A0=14, A1=15, dst)
    #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || \
        defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
        if (channel >= 14) channel -= 14;
    #endif

    // ATmega8: pin analog dimulai dari 14 (A0=14, A1=15, dst)
    #if defined(__AVR_ATmega8__)
        if (channel >= 14) channel -= 14; // Konversi ke channel 0-5
        if (channel > 5) channel = 5;     // Batasi maksimal channel 5
    #endif

    // Set channel di register ADMUX
    #if defined(MUX3)
        ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Untuk ADC > 8 channel
    #else
        ADMUX = (ADMUX & 0xE0) | (channel & 0x07); // Untuk ADC 8 channel
    #endif
}

uint16_t Analog::read() {
    // Mulai konversi ADC
    #if defined(ADSC)
        ADCSRA |= _BV(ADSC);
    #else
        ADCSRA |= (1 << ADSC);
    #endif

    // Tunggu hingga konversi selesai
    while (ADCSRA & _BV(ADSC));
    return ADC; // Baca hasil ADC
}

void Analog::setPrescaler(uint8_t factor) {
    // Reset bit prescaler
    ADCSRA &= ~(_BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));

    // Set prescaler sesuai input
    switch (factor) {
        case 2:  ADCSRA |= _BV(ADPS0); break;                    // 2
        case 4:  ADCSRA |= _BV(ADPS1); break;                    // 4
        case 8:  ADCSRA |= _BV(ADPS1) | _BV(ADPS0); break;       // 8
        case 16: ADCSRA |= _BV(ADPS2); break;                    // 16
        case 32: ADCSRA |= _BV(ADPS2) | _BV(ADPS0); break;       // 32
        case 64: ADCSRA |= _BV(ADPS2) | _BV(ADPS1); break;       // 64
        case 128: ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); break; // 128
        default: ADCSRA |= _BV(ADPS2) | _BV(ADPS1); break;       // Default 64
    }
}