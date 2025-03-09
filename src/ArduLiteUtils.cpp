#include "ArduLiteUtils.h"

void wait(uint16_t seconds) {
    while (seconds--) {
        for (long i = 0; i < 2500; i++) {
            asm(""); // NOP (No Operation) untuk delay
        }
    }
}

uint16_t testSpeed(uint8_t mode) {
    if (mode == 1) {
        TCCR1A = 0;
        TCCR1B = 1 << CS10;
        TCNT1 = 0;
        return 0;
    } else if (mode == 0) {
        return TCNT1;
    }
    return 0;
}