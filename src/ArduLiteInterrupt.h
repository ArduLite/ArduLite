#ifndef ARDULITE_INTERRUPT_H
#define ARDULITE_INTERRUPT_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ArduLiteTime.h"

class Interrupt {
    public:
        Interrupt(uint8_t interruptPin, void (*interruptFunction)(), uint8_t mode, uint16_t debounceDelay) {
            cli();
            initTimer(); // initTimer() sekarang aman dipanggil berulang
            if (interruptPin == 2) {
                DDRD &= ~(1 << PD2);
                PORTD |= (1 << PD2);
                
                isrCallback0 = interruptFunction;
                debounceTime0 = debounceDelay;
                lastInterruptTime0 = 0;
                MCUCR |= (mode << ISC00);

                #if defined(__AVR_ATmega8__)
                    GIMSK |= (1 << INT0);
                #elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
                    EIMSK |= (1 << INT0);
                #endif
            } else if (interruptPin == 3) {
                DDRD &= ~(1 << PD3);
                PORTD |= (1 << PD3);
                
                isrCallback1 = interruptFunction;
                debounceTime1 = debounceDelay;
                lastInterruptTime1 = 0;
                MCUCR |= (mode << ISC10);

                #if defined(__AVR_ATmega8__)
                    GIMSK |= (1 << INT1);
                #elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
                    EIMSK |= (1 << INT1);
                #endif
            }
            sei();
        }
    
        Interrupt(uint8_t interruptPin, void (*interruptFunction)(), uint8_t mode) 
            : Interrupt(interruptPin, interruptFunction, mode, 0) {}
    
        static void handleInt0() {
            if (debounceTime0 > 0) {
                // Akses langsung ke timer1_millis karena sudah dalam ISR (interrupts disabled)
                uint32_t currentTime = timer1_millis;
                if (currentTime - lastInterruptTime0 >= debounceTime0) {
                    lastInterruptTime0 = currentTime;
                    if (isrCallback0) isrCallback0();
                }
            } else {
                if (isrCallback0) isrCallback0();
            }
        }
    
        static void handleInt1() {
            if (debounceTime1 > 0) {
                uint32_t currentTime = timer1_millis;
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

void (*Interrupt::isrCallback0)() = nullptr;
void (*Interrupt::isrCallback1)() = nullptr;
uint32_t Interrupt::lastInterruptTime0 = 0;
uint32_t Interrupt::lastInterruptTime1 = 0;
uint16_t Interrupt::debounceTime0 = 0;
uint16_t Interrupt::debounceTime1 = 0;

ISR(INT0_vect) { Interrupt::handleInt0(); }
ISR(INT1_vect) { Interrupt::handleInt1(); }

#endif