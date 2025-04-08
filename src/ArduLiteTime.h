#ifndef ARDULITE_TIME_H
#define ARDULITE_TIME_H

#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint32_t timer2_millis;

uint32_t getTimeMs();
void initTimer();

volatile uint32_t timer2_millis = 0;

ISR(TIMER2_COMPA_vect) {
    timer2_millis++;
}

uint32_t getTimeMs() {
    uint32_t m;
    cli();
    m = timer2_millis;
    sei();
    return m;
}

void initTimer() {
    static bool timerInitialized = false;
    if (timerInitialized) return;
    timerInitialized = true;

    cli();
    
    // Reset Timer2 control registers
    TCCR2A = 0;
    TCCR2B = 0;
    
    // Configure Timer2 for CTC mode
    TCCR2A |= (1 << WGM21);
    
    // Set compare match value for approximately 1ms timing
    // (The exact value would depend on your clock frequency)
    OCR2A = 249;
    
    // Set prescaler to 64
    TCCR2B |= (1 << CS22);
    
    // Enable Timer2 compare match interrupt
    #if defined(__AVR_ATmega8__)
        TIMSK |= (1 << OCIE2);
    #elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
        TIMSK2 |= (1 << OCIE2A);
    #else
        #error "Mikrokontroler tidak didukung"
    #endif

    sei();
}

#endif