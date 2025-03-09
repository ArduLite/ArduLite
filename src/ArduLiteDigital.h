/*
    Created by Ajang Rahmat (email: ajangrahmat@gmail.com),
    Kelas Robot PT AViSha Inovasi Indonesia.
*/

#ifndef ARDULITE_DIGITAL_H
#define ARDULITE_DIGITAL_H

#include <avr/interrupt.h>

// Define an enum to specify pin direction
enum Direction {
    IN,  // Input mode
    OUT  // Output mode
};

// Class to control digital pins
class Digital {
private:
    uint8_t pin; // The pin number being used
public:
    // Constructor to initialize the digital pin
    Digital(uint8_t pin, Direction dir);

    // Function to turn the pin on (set HIGH)
    void on();

    // Function to turn the pin off (set LOW)
    void off();

    // Function to write a value to the pin (HIGH/LOW)
    void write(bool value);

    // Function to read the value from the pin
    bool read();

    // Function to toggle the pin (blink effect)
    void toggle();
};

#endif // ARDULITE_H