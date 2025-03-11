#ifndef ARDULITE_TIMER_INTERRUPT_H
#define ARDULITE_TIMER_INTERRUPT_H

#include <avr/io.h>
#include <avr/interrupt.h>

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
    
    // Konfigurasi Timer2 untuk base time 1ms
    TCCR2A = (1 << WGM21);   // CTC mode
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); // Prescaler 1024
    OCR2A = (F_CPU / 1024 / 1000) - 1; // 1ms interval
    TIMSK2 |= (1 << OCIE2A);
    
    sei();
}

uint8_t createTimer(uint32_t intervalMs, void (*callback)()) {
    if(timerCount >= MAX_TIMERS) return 255;
    
    // Inisialisasi manual tanpa designated initializer
    timers[timerCount].interval = intervalMs;
    timers[timerCount].counter = 0;
    timers[timerCount].callback = callback;
    timers[timerCount].enabled = true;
    
    return timerCount++;
}

void setTimerEnabled(uint8_t id, bool state) {
    if(id < MAX_TIMERS) {
        timers[id].enabled = state;
    }
}

void setTimerInterval(uint8_t id, uint32_t newInterval) {
    if(id < MAX_TIMERS) {
        timers[id].interval = newInterval;
        timers[id].counter = 0;
    }
}

ISR(TIMER2_COMPA_vect) {
    for(uint8_t i = 0; i < timerCount; i++) {
        if(timers[i].enabled) {
            timers[i].counter++;
            if(timers[i].counter >= timers[i].interval) {
                timers[i].counter = 0;
                if(timers[i].callback) timers[i].callback();
            }
        }
    }
}

#endif