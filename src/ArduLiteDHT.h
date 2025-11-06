#ifndef ARDULITE_DHT_H
#define ARDULITE_DHT_H

#include <avr/io.h>
#include <util/delay.h>

class DHT {
  private:
    uint8_t _pin;
    uint8_t _type;
    volatile uint8_t* _port;
    volatile uint8_t* _ddr;
    volatile uint8_t* _pin_reg;
    uint8_t _mask;
    
    float _temperature;
    float _humidity;
    
    void mapPin(uint8_t pin, volatile uint8_t** port, volatile uint8_t** ddr, volatile uint8_t** pin_reg, uint8_t* mask) {
      if (pin <= 7) { 
        *port = &PORTD; 
        *ddr = &DDRD; 
        *pin_reg = &PIND; 
        *mask = (1 << pin); 
      }
      else if (pin <= 13) { 
        *port = &PORTB; 
        *ddr = &DDRB; 
        *pin_reg = &PINB; 
        *mask = (1 << (pin - 8)); 
      }
      else { 
        *port = &PORTC; 
        *ddr = &DDRC; 
        *pin_reg = &PINC; 
        *mask = (1 << (pin - 14)); 
      }
    }
    
    void setOutput() {
      *_ddr |= _mask;
    }
    
    void setInput() {
      *_ddr &= ~_mask;
    }
    
    void setHigh() {
      *_port |= _mask;
    }
    
    void setLow() {
      *_port &= ~_mask;
    }
    
    bool readPin() {
      return (*_pin_reg & _mask) ? true : false;
    }
    
    uint8_t readByte() {
      uint8_t data = 0;
      for (int i = 0; i < 8; i++) {
        // Wait for the pin to go high
        while (!readPin());
        _delay_us(30);
        
        if (readPin()) {
          data |= (1 << (7 - i));
        }
        
        // Wait for the pin to go low
        while (readPin());
      }
      return data;
    }

  public:
    // Konstruktor - type: 11 untuk DHT11, 22 untuk DHT22
    DHT(uint8_t pin, uint8_t type) {
      _pin = pin;
      _type = type;
      
      mapPin(_pin, &_port, &_ddr, &_pin_reg, &_mask);
      
      // Set pin as input dengan pull-up
      setInput();
      setHigh();
      
      _temperature = 0.0;
      _humidity = 0.0;
    }
    
    // Fungsi untuk membaca data dari sensor
    int read() {
      uint8_t data[5] = {0, 0, 0, 0, 0};
      
      // Kirim start signal
      setOutput();
      setLow();
      _delay_ms(18);
      setInput();
      _delay_us(40);
      
      // Tunggu respons dari DHT
      if (!readPin()) {
        _delay_us(80);
        if (readPin()) {
          _delay_us(80);
          
          // Baca 40 bit data
          for (int i = 0; i < 5; i++) {
            data[i] = readByte();
          }
          
          // Verifikasi checksum
          if (data[4] == (uint8_t)(data[0] + data[1] + data[2] + data[3])) {
            if (_type == 11) {
              // DHT11
              _humidity = data[0];
              _temperature = data[2];
            } else {
              // DHT22
              _humidity = ((data[0] << 8) | data[1]) / 10.0;
              _temperature = (((data[2] & 0x7F) << 8) | data[3]) / 10.0;
              if (data[2] & 0x80) {
                _temperature = -_temperature;
              }
            }
            return 0; // Success
          }
        }
      }
      return -1; // Error
    }
    
    // Fungsi untuk membaca suhu
    float readTemperature(bool fahrenheit = false) {
      if (fahrenheit) {
        return _temperature * 9.0 / 5.0 + 32.0;
      }
      return _temperature;
    }
    
    // Fungsi untuk membaca kelembapan
    float readHumidity() {
      return _humidity;
    }
    
    // Fungsi untuk membaca kedua data sekaligus
    bool readBoth(float &temperature, float &humidity, bool fahrenheit = false) {
      int result = read();
      if (result == 0) {
        temperature = readTemperature(fahrenheit);
        humidity = readHumidity();
        return true;
      }
      return false;
    }
};

#endif