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

ArduLiteI2C i2c;

void ArduLiteI2C::begin() {
  TWSR = 0x00;
  TWBR = 72;
  TWCR = (1 << TWEN);
}

void ArduLiteI2C::read(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len) {
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  TWDR = (addr << 1);
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  TWDR = reg;
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  TWDR = (addr << 1) | 1;
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));
  for (uint8_t i = 0; i < len; i++) {
    TWCR = (1 << TWEN) | (1 << TWINT) | ((i < len - 1) ? (1 << TWEA) : 0);
    while (!(TWCR & (1 << TWINT)));
    data[i] = TWDR;
  }
  TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
}

void ArduLiteI2C::write(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len) {
  TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));

  TWDR = (addr << 1);
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));

  TWDR = reg;
  TWCR = (1 << TWEN) | (1 << TWINT);
  while (!(TWCR & (1 << TWINT)));

  for (uint8_t i = 0; i < len; i++) {
    TWDR = data[i];
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)));
  }

  TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
  _delay_ms(1);
}

uint8_t bcdToDec(uint8_t val) {
  return ((val / 16 * 10) + (val % 16));
}

uint8_t decToBcd(uint8_t val) {
  return ((val / 10 * 16) + (val % 10));
}

#endif