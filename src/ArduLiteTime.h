#ifndef ARDULITE_TIME_H
#define ARDULITE_TIME_H

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t timer1_millis = 0;  // Ganti dari timer0_millis ke timer1_millis

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
    cli();
    
    TCCR1B |= (1 << WGM12); // CTC Mode
    OCR1A = 15624;          // 1 ms interrupt (dengan prescaler 64)
    TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler 64
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 Compare A Match Interrupt

    sei();
}

#endif
