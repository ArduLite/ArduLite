#ifndef ARDULITE_INTERRUPT_H
#define ARDULITE_INTERRUPT_H

#include <avr/io.h>
#include <avr/interrupt.h>

// ✅ Simulated millis() function renamed to getTimeMs()
volatile uint32_t timer0_millis = 0;

ISR(TIMER0_COMPA_vect) {
    timer0_millis++;
}

uint32_t getTimeMs() {  // 🔄 Replaces millis()
    uint32_t m;
    cli();
    m = timer0_millis;
    sei();
    return m;
}

// ✅ Initialize Timer0 for getTimeMs()
void initTimer() {  
    cli();
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);
    OCR0A = 249;
    TIMSK0 |= (1 << OCIE0A);
    sei();
}

class Interrupt {
    public:
        // Dengan debounce (menggunakan timer)
        Interrupt(uint8_t interruptPin, void (*interruptFunction)(), uint8_t mode, uint16_t debounceDelay) {
            static bool initialized = false;
            if (!initialized) {
                initTimer(); // Inisialisasi timer hanya sekali
                initialized = true;
            }
    
            cli();
            if (interruptPin == 2) {
                isrCallback0 = interruptFunction;
                debounceTime0 = debounceDelay;
                lastInterruptTime0 = 0;
                EICRA |= (mode << ISC00);
                EIMSK |= (1 << INT0);
            } else if (interruptPin == 3) {
                isrCallback1 = interruptFunction;
                debounceTime1 = debounceDelay;
                lastInterruptTime1 = 0;
                EICRA |= (mode << ISC10);
                EIMSK |= (1 << INT1);
            }
            sei();
        }
    
        // Tanpa debounce (langsung eksekusi)
        Interrupt(uint8_t interruptPin, void (*interruptFunction)(), uint8_t mode) {
            cli();
            if (interruptPin == 2) {
                isrCallback0 = interruptFunction;
                EICRA |= (mode << ISC00);
                EIMSK |= (1 << INT0);
            } else if (interruptPin == 3) {
                isrCallback1 = interruptFunction;
                EICRA |= (mode << ISC10);
                EIMSK |= (1 << INT1);
            }
            sei();
        }
    
        static void handleInt0() {
            if (debounceTime0 > 0) { // Cek apakah debounce digunakan
                uint32_t currentTime = getTimeMs();
                if (currentTime - lastInterruptTime0 >= debounceTime0) {
                    lastInterruptTime0 = currentTime;
                    if (isrCallback0) isrCallback0();
                }
            } else {
                if (isrCallback0) isrCallback0();
            }
        }
    
        static void handleInt1() {
            if (debounceTime1 > 0) { // Cek apakah debounce digunakan
                uint32_t currentTime = getTimeMs();
                if (currentTime - lastInterruptTime1 >= debounceTime1) {
                    lastInterruptTime1 = currentTime;
                    if (isrCallback1) isrCallback1();
                }
            } else {
                if (isrCallback1) isrCallback1();
            }
        }
    
    private:
        static uint32_t lastInterruptTime0;
        static uint32_t lastInterruptTime1;
        static uint16_t debounceTime0;
        static uint16_t debounceTime1;
        static void (*isrCallback0)();
        static void (*isrCallback1)();
    };
    
    // Definisi variabel statis
    void (*Interrupt::isrCallback0)() = nullptr;
    void (*Interrupt::isrCallback1)() = nullptr;
    uint32_t Interrupt::lastInterruptTime0 = 0;
    uint32_t Interrupt::lastInterruptTime1 = 0;
    uint16_t Interrupt::debounceTime0 = 0;
    uint16_t Interrupt::debounceTime1 = 0;
    
    // ISR
    ISR(INT0_vect) { Interrupt::handleInt0(); }
    ISR(INT1_vect) { Interrupt::handleInt1(); }
    

#endif
