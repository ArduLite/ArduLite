#ifndef ARDULITE_UTILS_H
#define ARDULITE_UTILS_H

#include <stdint.h>
#include <avr/io.h>

void wait(uint16_t seconds);

uint16_t testSpeed(uint8_t mode);

#endif // ARDULITE_UTILS_H