#include "ArduLiteUtils.h"

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