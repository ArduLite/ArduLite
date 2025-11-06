// ArduLiteI2C.h
#ifndef ARDULITE_I2C_H
#define ARDULITE_I2C_H

#include <avr/io.h>
#include <util/delay.h>

class ArduLiteI2C {
  public:
    void begin();
    void read(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len);
    void write(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len);
};

extern ArduLiteI2C i2c;

// Implementasi
void ArduLiteI2C::begin() {
  TWSR = 0x00;           // Prescaler 1
  TWBR = 72;             // 100kHz pada 16MHz
  TWCR = (1 << TWEN);    // Enable I2C
}

void ArduLiteI2C::read(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len) {
  // Start condition
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  
  // Send address + write
  TWDR = (addr << 1);
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  
  // Send register address
  TWDR = reg;
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  
  // Repeated start
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  
  // Send address + read
  TWDR = (addr << 1) | 1;
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  
  // Read data
  for (uint8_t i = 0; i < len; i++) {
    if (i == len - 1) {
      // NACK pada byte terakhir
      TWCR = (1 << TWEN) | (1 << TWINT);
    } else {
      // ACK untuk byte selanjutnya
      TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
    }
    while (!(TWCR & (1 << TWINT)));
    data[i] = TWDR;
  }
  
  // Stop condition
  TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
}

void ArduLiteI2C::write(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len) {
  // Start condition
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  
  // Send address + write
  TWDR = (addr << 1);
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  
  // Send register address
  TWDR = reg;
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  
  // Send data
  for (uint8_t i = 0; i < len; i++) {
    TWDR = data[i];
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
  }
  
  // Stop condition
  TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
  _delay_ms(1);
}

ArduLiteI2C i2c;

#endif