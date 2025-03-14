#ifndef ARDULITE_TIMER_INTERRUPT_H
#define ARDULITE_TIMER_INTERRUPT_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>  // Untuk tipe bool

#define MAX_TIMERS 4

typedef struct {
    volatile uint32_t interval;
    volatile uint32_t counter;
    volatile void (*callback)();
    volatile bool enabled;
} VirtualTimer;

volatile static VirtualTimer timers[MAX_TIMERS];
volatile static uint8_t timerCount = 0;

void initTimerInterrupt() {
    cli();
    #if defined(__AVR_ATmega8__)
        // Konfigurasi untuk ATmega8: Timer2 CTC mode dengan prescaler 1024
        TCCR2 = (1 << WGM21) | (1 << CS22) | (1 << CS21) | (1 << CS20);
        OCR2 = (F_CPU / 1024 / 1000) - 1;  // Interval 1ms
        TIMSK |= (1 << OCIE2);
    #elif defined(__AVR_ATmega328P__)
        // Konfigurasi untuk ATmega328P: Timer2 CTC mode dengan prescaler 1024
        TCCR2A = (1 << WGM21);
        TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
        OCR2A = (F_CPU / 1024 / 1000) - 1;  // Interval 1ms
        TIMSK2 |= (1 << OCIE2A);
    #else
        #error "Mikrokontroler tidak didukung"
    #endif
    sei();
}

uint8_t createTimer(uint32_t intervalMs, void (*callback)()) {
    if (timerCount >= MAX_TIMERS) return 255;
    
    timers[timerCount].interval = intervalMs;
    timers[timerCount].counter = 0;
    timers[timerCount].callback = callback;
    timers[timerCount].enabled = true;
    
    return timerCount++;
}

void setTimerEnabled(uint8_t id, bool state) {
    if (id < MAX_TIMERS) {
        timers[id].enabled = state;
    }
}

void setTimerInterval(uint8_t id, uint32_t newInterval) {
    if (id < MAX_TIMERS) {
        timers[id].interval = newInterval;
        timers[id].counter = 0;
    }
}

#if defined(__AVR_ATmega8__)
ISR(TIMER2_COMP_vect) {
#elif defined(__AVR_ATmega328P__)
ISR(TIMER2_COMPA_vect) {
#endif
    for (uint8_t i = 0; i < timerCount; i++) {
        if (timers[i].enabled) {
            timers[i].counter++;
            if (timers[i].counter >= timers[i].interval) {
                timers[i].counter = 0;
                if (timers[i].callback) timers[i].callback();
            }
        }
    }
}

#endif
