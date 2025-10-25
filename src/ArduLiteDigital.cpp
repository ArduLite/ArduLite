/*
    Created by Ajang Rahmat (email: ajangrahmat@gmail.com),
    Kelas Robot PT AViSha Inovasi Indonesia.
*/

#include "ArduLiteDigital.h"
#include <util/delay.h>

// Constructor with default active HIGH
Digital::Digital(uint8_t pin, Direction dir) {
    this->pin = pin;
    this->activeLow = false; // Default active HIGH
    setupPin(dir);
}

// Constructor with specified active state
Digital::Digital(uint8_t pin, Direction dir, ActiveState activeState) {
    this->pin = pin;
    this->activeLow = (activeState == ACTIVE_OFF);
    setupPin(dir);
}

// Common pin setup function
void Digital::setupPin(Direction dir) {
    // Set the pin as output or input
    if (dir == OUT) {

    //defined for ATmega48, ATmega88, ATmega168, ATmega328P, ATmega328PB
    #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
        if (pin >= 14 && pin <= 19) {
            // Pin is in PORTC (A0-A5)
            DDRC |= (1 << (pin - 14));
        } else if (pin >= 8) {
            // Pin is in PORTB (8-13)
            DDRB |= (1 << (pin - 8));
        } else {
            // Pin is in PORTD (0-7)
            DDRD |= (1 << pin);
        }
    #endif

    //defined for ATmega8, ATmega16, ATmega32
    #if defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
        if (pin >= 0 && pin <= 7) {
            // Pin is in PORTD (0-7) - Corrected from PORTB
            DDRD |= (1 << pin);
        } else if (pin >= 8 && pin <= 13) {
            // Pin is in PORTB (8-13) - Corrected from PORTD
            DDRB |= (1 << (pin - 8));
        } else if (pin >= 14 && pin <= 19) { // Corrected range from 21 to 19
            // Pin is in PORTC (A0-A5)
            DDRC |= (1 << (pin - 14));
        }
    #endif

    //defined for ATmega1280, ATmega2560
    #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        if (pin >= 22 && pin <= 29) {
            // Pin is in PORTA (22-29)
            DDRA |= (1 << (pin - 22));
        } else if (pin >= 0 && pin <= 7) {
            // Pin is in PORTG (0-7)
            DDRG |= (1 << pin);
        } else if (pin >= 8 && pin <= 13) {
            // Pin is in PORTH (8-13)
            DDRH |= (1 << (pin - 8));
        } else if (pin >= 14 && pin <= 21) {
            // Pin is in PORTJ (14-21)
            DDRJ |= (1 << (pin - 14));
        } else if (pin >= 38 && pin <= 41) {
            // Pin is in PORTL (38-41)
            DDRL |= (1 << (pin - 38));
        }
    #endif

    } else if (dir == IN) {
        if (pin >= 14 && pin <= 19) {
            DDRC &= ~(1 << (pin - 14));
        } else if (pin >= 8) {
            DDRB &= ~(1 << (pin - 8));
        } else {
            DDRD &= ~(1 << pin);
        }
    }
}

// Function to turn the pin on (active state)
void Digital::on() {
    if (activeLow) {
        // For active LOW, set pin to LOW to turn on
        //defined for ATmega48, ATmega88, ATmega168, ATmega328P, ATmega328PB
        #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
            if (pin >= 14 && pin <= 19) {
                PORTC &= ~(1 << (pin - 14));  // Set pin LOW on PORTC
            } else if (pin >= 8) {
                PORTB &= ~(1 << (pin - 8));  // Set pin LOW on PORTB
            } else {
                PORTD &= ~(1 << pin);  // Set pin LOW on PORTD
            }
        #endif

        //defined for ATmega8, ATmega16, ATmega32
        #if defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
            if (pin >= 0 && pin <= 7) {
                PORTD &= ~(1 << pin);  // Corrected from PORTB
            } else if (pin >= 8 && pin <= 13) {
                PORTB &= ~(1 << (pin - 8));  // Corrected from PORTD
            } else if (pin >= 14 && pin <= 19) { // Corrected range
                PORTC &= ~(1 << (pin - 14));
            }
        #endif

        //defined for ATmega1280, ATmega2560
        #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
            if (pin >= 22 && pin <= 29) {
                PORTA &= ~(1 << (pin - 22));  // Set pin LOW on PORTA
            } else if (pin >= 0 && pin <= 7) {
                PORTG &= ~(1 << pin);  // Set pin LOW on PORTG
            } else if (pin >= 8 && pin <= 13) {
                PORTH &= ~(1 << (pin - 8));  // Set pin LOW on PORTH
            } else if (pin >= 14 && pin <= 21) {
                PORTJ &= ~(1 << (pin - 14));  // Set pin LOW on PORTJ
            } else if (pin >= 38 && pin <= 41) {
                PORTL &= ~(1 << (pin - 38));  // Set pin LOW on PORTL
            }
        #endif
    } else {
        // For active HIGH, set pin to HIGH to turn on
        //defined for ATmega48, ATmega88, ATmega168, ATmega328P, ATmega328PB
        #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
            if (pin >= 14 && pin <= 19) {
                PORTC |= (1 << (pin - 14));  // Set pin HIGH on PORTC
            } else if (pin >= 8) {
                PORTB |= (1 << (pin - 8));  // Set pin HIGH on PORTB
            } else {
                PORTD |= (1 << pin);  // Set pin HIGH on PORTD
            }
        #endif

        //defined for ATmega8, ATmega16, ATmega32
        #if defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
            if (pin >= 0 && pin <= 7) {
                PORTD |= (1 << pin);  // Corrected from PORTB
            } else if (pin >= 8 && pin <= 13) {
                PORTB |= (1 << (pin - 8));  // Corrected from PORTD
            } else if (pin >= 14 && pin <= 19) { // Corrected range
                PORTC |= (1 << (pin - 14));
            }
        #endif

        //defined for ATmega1280, ATmega2560
        #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
            if (pin >= 22 && pin <= 29) {
                PORTA |= (1 << (pin - 22));  // Set pin HIGH on PORTA
            } else if (pin >= 0 && pin <= 7) {
                PORTG |= (1 << pin);  // Set pin HIGH on PORTG
            } else if (pin >= 8 && pin <= 13) {
                PORTH |= (1 << (pin - 8));  // Set pin HIGH on PORTH
            } else if (pin >= 14 && pin <= 21) {
                PORTJ |= (1 << (pin - 14));  // Set pin HIGH on PORTJ
            } else if (pin >= 38 && pin <= 41) {
                PORTL |= (1 << (pin - 38));  // Set pin HIGH on PORTL
            }
        #endif
    }
}

// Function to turn the pin off (inactive state)
void Digital::off() {
    if (activeLow) {
        // For active LOW, set pin to HIGH to turn off
        //defined for ATmega48, ATmega88, ATmega168, ATmega328P, ATmega328PB
        #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
            if (pin >= 14 && pin <= 19) {
                PORTC |= (1 << (pin - 14));  // Set pin HIGH on PORTC
            } else if (pin >= 8) {
                PORTB |= (1 << (pin - 8));  // Set pin HIGH on PORTB
            } else {
                PORTD |= (1 << pin);  // Set pin HIGH on PORTD
            }
        #endif

        //defined for ATmega8, ATmega16, ATmega32
        #if defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
            if (pin >= 0 && pin <= 7) {
                PORTD |= (1 << pin);  // Corrected from PORTB
            } else if (pin >= 8 && pin <= 13) {
                PORTB |= (1 << (pin - 8));  // Corrected from PORTD
            } else if (pin >= 14 && pin <= 19) { // Corrected range
                PORTC |= (1 << (pin - 14));
            }
        #endif

        //defined for ATmega1280, ATmega2560
        #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
            if (pin >= 22 && pin <= 29) {
                PORTA |= (1 << (pin - 22));  // Set pin HIGH on PORTA
            } else if (pin >= 0 && pin <= 7) {
                PORTG |= (1 << pin);  // Set pin HIGH on PORTG
            } else if (pin >= 8 && pin <= 13) {
                PORTH |= (1 << (pin - 8));  // Set pin HIGH on PORTH
            } else if (pin >= 14 && pin <= 21) {
                PORTJ |= (1 << (pin - 14));  // Set pin HIGH on PORTJ
            } else if (pin >= 38 && pin <= 41) {
                PORTL |= (1 << (pin - 38));  // Set pin HIGH on PORTL
            }
        #endif
    } else {
        // For active HIGH, set pin to LOW to turn off
        //defined for ATmega48, ATmega88, ATmega168, ATmega328P, ATmega328PB
        #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
            if (pin >= 14 && pin <= 19) {
                PORTC &= ~(1 << (pin - 14));  // Set pin LOW on PORTC
            } else if (pin >= 8) {
                PORTB &= ~(1 << (pin - 8));  // Set pin LOW on PORTB
            } else {
                PORTD &= ~(1 << pin);  // Set pin LOW on PORTD
            }
        #endif

        //defined for ATmega8, ATmega16, ATmega32
        #if defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
            if (pin >= 0 && pin <= 7) {
                PORTD &= ~(1 << pin);  // Corrected from PORTB
            } else if (pin >= 8 && pin <= 13) {
                PORTB &= ~(1 << (pin - 8));  // Corrected from PORTD
            } else if (pin >= 14 && pin <= 19) { // Corrected range
                PORTC &= ~(1 << (pin - 14));
            }
        #endif

        //defined for ATmega1280, ATmega2560
        #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
            if (pin >= 22 && pin <= 29) {
                PORTA &= ~(1 << (pin - 22));  // Set pin LOW on PORTA
            } else if (pin >= 0 && pin <= 7) {
                PORTG &= ~(1 << pin);  // Set pin LOW on PORTG
            } else if (pin >= 8 && pin <= 13) {
                PORTH &= ~(1 << (pin - 8));  // Set pin LOW on PORTH
            } else if (pin >= 14 && pin <= 21) {
                PORTJ &= ~(1 << (pin - 14));  // Set pin LOW on PORTJ
            } else if (pin >= 38 && pin <= 41) {
                PORTL &= ~(1 << (pin - 38));  // Set pin LOW on PORTL
            }
        #endif
    }
}

// Function to write a value to the pin (considering active state)
void Digital::write(bool value) {
    if (value) {
        on();
    } else {
        off();
    }
}

// Function to set state (ON/OFF)
void Digital::turn(State state) {
    write(state == ON);
}

// Function to generate a pulse
void Digital::pulse(unsigned int lowTime, unsigned int highTime) {
    turn(OFF);
    
    // Delay for lowTime microseconds
    for (unsigned int i = 0; i < lowTime; i++) {
        _delay_us(1);
    }
    
    turn(ON);
    
    // Delay for highTime microseconds
    for (unsigned int i = 0; i < highTime; i++) {
        _delay_us(1);
    }
    
    turn(OFF);
}

// Function to read the pin state (considering active state)
bool Digital::read() {
    bool physicalState;
    
    // ATmega48, ATmega88, ATmega168, ATmega328P, ATmega328PB
    #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
        if (pin >= 14 && pin <= 19) {
            physicalState = (PINC & (1 << (pin - 14))) != 0;
        } else if (pin >= 8) {
            physicalState = (PINB & (1 << (pin - 8))) != 0;
        } else {
            physicalState = (PIND & (1 << pin)) != 0;
        }
    #endif

    // ATmega8, ATmega16, ATmega32
    #if defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
        if (pin >= 0 && pin <= 7) {
            physicalState = (PIND & (1 << pin)) != 0;
        } else if (pin >= 8 && pin <= 13) {
            physicalState = (PINB & (1 << (pin - 8))) != 0;
        } else if (pin >= 14 && pin <= 19) {
            physicalState = (PINC & (1 << (pin - 14))) != 0;
        }
    #endif

    // ATmega1280, ATmega2560
    #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        if (pin >= 22 && pin <= 29) {
            physicalState = (PINA & (1 << (pin - 22))) != 0;
        } else if (pin >= 0 && pin <= 7) {
            physicalState = (PING & (1 << pin)) != 0;
        } else if (pin >= 8 && pin <= 13) {
            physicalState = (PINH & (1 << (pin - 8))) != 0;
        } else if (pin >= 14 && pin <= 21) {
            physicalState = (PINJ & (1 << (pin - 14))) != 0;
        } else if (pin >= 38 && pin <= 41) {
            physicalState = (PINL & (1 << (pin - 38))) != 0;
        }
    #endif

    // If active LOW, invert the logical state
    if (activeLow) {
        return !physicalState;
    }
    
    return physicalState;
}

// Function to toggle the pin
void Digital::toggle() {
    write(!read());
}

// Function to set active state
void Digital::setActiveState(ActiveState activeState) {
    this->activeLow = (activeState == ACTIVE_OFF);
}