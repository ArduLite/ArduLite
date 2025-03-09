#ifndef ArduLiteAnalog_h
#define ArduLiteAnalog_h

#include <avr/io.h>

class Analog {
public:
    Analog(uint8_t pin);
    uint16_t read();
private:
    uint8_t _pin;
};

#endif
