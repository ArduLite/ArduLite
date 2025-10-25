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

// Define an enum for active state (gunakan nama berbeda untuk menghindari konflik)
enum ActiveState {
    ACTIVE_ON, 
    ACTIVE_OFF   
};

// Define states for turn() method
enum State {
    OFF = 0,
    ON = 1
};

// Class to control digital pins
class Digital {
private:
    uint8_t pin; // The pin number being used
    bool activeLow; // Whether the pin is active LOW
    
    // Private function to setup pin direction
    void setupPin(Direction dir);
    
public:
    // Constructors
    Digital(uint8_t pin, Direction dir); // Default: active HIGH
    Digital(uint8_t pin, Direction dir, ActiveState activeState);

    // Function to turn the pin on (set to active state)
    void on();

    // Function to turn the pin off (set to inactive state)
    void off();

    // Function to write a value to the pin (considering active state)
    void write(bool value);

    // Function to set state (ON/OFF) - new method
    void turn(State state);

    // Function to generate a pulse (new method)
    void pulse(unsigned int lowTime, unsigned int highTime);

    // Function to read the value from the pin
    bool read();

    // Function to toggle the pin (blink effect)
    void toggle();
    
    // Function to set active state
    void setActiveState(ActiveState activeState);
};

#endif // ARDULITE_H