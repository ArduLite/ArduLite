// ArduLiteTime.h
#ifndef ARDULITE_TIME_H
#define ARDULITE_TIME_H

#include <avr/io.h>
#include <avr/interrupt.h>

// Enum untuk memilih resolusi timer
enum TimeResolution {
  MILLI,  // Resolusi milidetik (1ms)
  MICRO   // Resolusi mikrodetik (presisi lebih tinggi)
};

volatile uint32_t _timer2_millis = 0;
volatile uint32_t _timer2_micros = 0;
volatile uint8_t _timer2_fract = 0;

ISR(TIMER2_COMPA_vect) {
    _timer2_millis++;
    
    // Update counter mikrodetik dengan presisi lebih tinggi
    _timer2_micros += 1000;
    _timer2_fract += 24;  // 0.024 ms per interrupt untuk koreksi
    if (_timer2_fract >= 250) {
        _timer2_fract -= 250;
        _timer2_micros += 1;
    }
}

ISR(TIMER2_OVF_vect) {
    // Overflow interrupt untuk mode MICRO (presisi tinggi)
    _timer2_micros += 4;  // Setiap overflow ~4us @ 16MHz dengan prescaler 64
}

class Time {
  private:
    TimeResolution resolution;
    bool initialized = false;
    
    void initMilli() {
      cli();
      TCCR2A = 0;
      TCCR2B = 0;
      TCCR2A |= (1 << WGM21);  // CTC mode
      OCR2A = 249;              // 1ms interval @ 16MHz/64
      TCCR2B |= (1 << CS22);    // Prescaler 64
      
      #if defined(__AVR_ATmega8__)
          TIMSK |= (1 << OCIE2);
      #elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
          TIMSK2 |= (1 << OCIE2A);
      #else
          #error "Mikrokontroler tidak didukung"
      #endif
      sei();
    }
    
    void initMicro() {
      cli();
      TCCR2A = 0;
      TCCR2B = 0;
      TCCR2A |= (1 << WGM21);  // CTC mode
      OCR2A = 249;              // Base timing
      TCCR2B |= (1 << CS22);    // Prescaler 64
      
      #if defined(__AVR_ATmega8__)
          TIMSK |= (1 << OCIE2) | (1 << TOIE2);
      #elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
          TIMSK2 |= (1 << OCIE2A) | (1 << TOIE2);  // Compare match + overflow
      #else
          #error "Mikrokontroler tidak didukung"
      #endif
      sei();
    }
    
  public:
    // Constructor dengan pilihan resolusi (default: MILLI)
    Time(TimeResolution res = MILLI) : resolution(res) {
      if (!initialized) {
        initialized = true;
        if (resolution == MILLI) {
          initMilli();
        } else {
          initMicro();
        }
      }
    }
    
    // Baca waktu dalam milidetik
    uint32_t millis() {
      uint32_t m;
      cli();
      m = _timer2_millis;
      sei();
      return m;
    }
    
    // Baca waktu dalam mikrodetik
    uint32_t micros() {
      uint32_t m;
      cli();
      m = _timer2_micros;
      sei();
      return m;
    }
    
    // Alias untuk kompatibilitas (menggunakan resolusi yang dipilih)
    uint32_t now() {
      return (resolution == MILLI) ? millis() : micros();
    }
    
    // Tunggu sekian milidetik
    void wait(uint32_t ms) {
      uint32_t start = millis();
      while (millis() - start < ms);
    }
    
    // Tunggu sekian mikrodetik (software delay)
    void waitMicros(uint16_t us) {
      #if F_CPU == 16000000L
        // 16 MHz
        uint16_t loops = us * 4;
        while (loops--) {
          __asm__ __volatile__ ("nop");
        }
      #elif F_CPU == 8000000L
        // 8 MHz
        uint16_t loops = us * 2;
        while (loops--) {
          __asm__ __volatile__ ("nop");
        }
      #else
        // Fallback untuk clock speed lain
        while (us--) {
          __asm__ __volatile__ (
            "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
            "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
            "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t"
          );
        }
      #endif
    }
    
    // Delay presisi tinggi menggunakan hardware counter (untuk mode MICRO)
    void delayMicros(uint16_t us) {
      if (resolution == MICRO) {
        uint32_t start = micros();
        while (micros() - start < us);
      } else {
        waitMicros(us);  // Fallback ke software delay
      }
    }
    
    // Helper untuk mengecek resolusi
    TimeResolution getResolution() {
      return resolution;
    }
};

#endif