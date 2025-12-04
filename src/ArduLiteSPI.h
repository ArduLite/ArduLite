// ArduLiteSPI.h
#ifndef ARDULITE_SPI_H
#define ARDULITE_SPI_H

#include <avr/io.h>
#include <util/delay.h>

// Pin definitions
#define MOSI_PIN PB3
#define MISO_PIN PB4
#define SCK_PIN PB5

class ArduLiteSPI {
public:
    // Inisialisasi SPI
    void begin() {
        // Set MOSI, SCK sebagai output
        DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN);
        // Set MISO sebagai input
        DDRB &= ~(1 << MISO_PIN);
        // Enable SPI, Master, clock rate fck/16
        SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    }
    
    // Transfer data SPI
    unsigned char transfer(unsigned char data) {
        SPDR = data;
        while (!(SPSR & (1 << SPIF)));
        return SPDR;
    }
    
    // Set pin sebagai CS (Chip Select)
    void setCS(unsigned char pin) {
        if (pin >= 8 && pin <= 13) {
            // Port B (Pin 8-13)
            DDRB |= (1 << (pin - 8));
            PORTB |= (1 << (pin - 8)); // Set HIGH
        } else if (pin >= 0 && pin <= 7) {
            // Port D (Pin 0-7)
            DDRD |= (1 << pin);
            PORTD |= (1 << pin); // Set HIGH
        }
    }
    
    // CS LOW
    void selectDevice(unsigned char pin) {
        if (pin >= 8 && pin <= 13) {
            PORTB &= ~(1 << (pin - 8));
        } else if (pin >= 0 && pin <= 7) {
            PORTD &= ~(1 << pin);
        }
    }
    
    // CS HIGH
    void deselectDevice(unsigned char pin) {
        if (pin >= 8 && pin <= 13) {
            PORTB |= (1 << (pin - 8));
        } else if (pin >= 0 && pin <= 7) {
            PORTD |= (1 << pin);
        }
    }
};

#endif // ARDULITE_SPI_H