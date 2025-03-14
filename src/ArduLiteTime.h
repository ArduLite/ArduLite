#ifndef ARDULITE_TIME_H
#define ARDULITE_TIME_H

#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint32_t timer1_millis;

uint32_t getTimeMs();
void initTimer();

volatile uint32_t timer1_millis = 0;

ISR(TIMER1_COMPA_vect) {
    timer1_millis++;
}

uint32_t getTimeMs() {
    uint32_t m;
    cli();
    m = timer1_millis;
    sei();
    return m;
}

void initTimer() {
    static bool timerInitialized = false; // Pastikan inisialisasi hanya sekali
    if (timerInitialized) return;
    timerInitialized = true;

    cli();
    
    TCCR1A = 0;
    TCCR1B = 0;
    
    TCCR1B |= (1 << WGM12);
    OCR1A = 249;
    TCCR1B |= (1 << CS11) | (1 << CS10);
    
    #if defined(__AVR_ATmega8__)
        TIMSK |= (1 << OCIE1A);
    #elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
        TIMSK1 |= (1 << OCIE1A);
    #else
        #error "Mikrokontroler tidak didukung"
    #endif

    sei();
}

#endif