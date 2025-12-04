#ifndef ARDULITE_UART_H
#define ARDULITE_UART_H

#ifdef DEBUG
    #define debug(x) uart.send(x)
    #define debugLine(x) uart.sendLine(x)    
#else
    #define debug(x)
    #define debugLine(x)
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <stdlib.h>  // Untuk itoa() dan utoa()

#define F_CPU 16000000UL  // Frekuensi clock mikrokontroler (16 MHz)

// String literal disimpan di PROGMEM agar tidak menggunakan SRAM
const char trueStr[] PROGMEM  = "true";
const char falseStr[] PROGMEM = "false";
const char newLine[] PROGMEM  = "\r\n";

class UART {
public:
    UART(uint8_t uartNum);

    void begin(unsigned long baud, void (*callback)(const char *) = nullptr);
    
    void send(char data);
    void send(const char *str);
    void send(int32_t data);
    void send(uint32_t data);
    void send(uint16_t data);
    void send(int16_t data);
    void send(uint8_t data);
    void send(int8_t data);
    void send(float data);
    void send(bool data);
    
    void sendLine(char data);  // TAMBAHAN: Overload untuk char
    void sendLine(const char *str);
    void sendLine(int32_t data);
    void sendLine(uint32_t data);
    void sendLine(uint16_t data);
    void sendLine(int16_t data);
    void sendLine(uint8_t data);
    void sendLine(int8_t data);
    void sendLine(float data);
    void sendLine(bool data);

    void handleInterrupt();

private:
    void init(unsigned long baud);
    void receive(char received);
    void send_P(const char *progmem_str);  // Fungsi pembantu untuk mengirim string dari PROGMEM

    uint8_t uartNumber;
    volatile uint8_t *ubrrh;
    volatile uint8_t *ubrrl;
    volatile uint8_t *ucsrb;
    volatile uint8_t *ucsrc;
    volatile uint8_t *udr;
    volatile uint8_t *ucsra;

    char buffer[16];  // Buffer dikurangi dari 32 menjadi 16 untuk menghemat RAM
    uint8_t index = 0;
    void (*data_callback)(const char *);
};


extern UART uart;
#if defined(__AVR_ATmega2560__)
extern UART uart1;
extern UART uart2;
extern UART uart3;
#endif

UART uart(0);
#if defined(__AVR_ATmega2560__)
UART uart1(1);
UART uart2(2);
UART uart3(3);
#endif

UART::UART(uint8_t uartNum) {
    uartNumber = uartNum;
    data_callback = nullptr;

    #if defined(__AVR_ATmega8__)
        ubrrh = &UBRRH;
        ubrrl = &UBRRL;
        ucsrb = &UCSRB;
        ucsrc = &UCSRC;
        udr   = &UDR;
        ucsra = &UCSRA;
    #endif

    #if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
        // Registers for ATmega48, ATmega88, ATmega168, ATmega328P
        ubrrh = &UBRR0H;
        ubrrl = &UBRR0L;
        ucsrb = &UCSR0B;
        ucsrc = &UCSR0C;
        udr   = &UDR0;
        ucsra = &UCSR0A;
    #endif

    #if defined(__AVR_ATmega2560__)
        // Registers for ATmega2560 (multiple UARTs)
        if (uartNum == 1) {
            ubrrh = &UBRR1H;
            ubrrl = &UBRR1L;
            ucsrb = &UCSR1B;
            ucsrc = &UCSR1C;
            udr   = &UDR1;
            ucsra = &UCSR1A;
        } else if (uartNum == 2) {
            ubrrh = &UBRR2H;
            ubrrl = &UBRR2L;
            ucsrb = &UCSR2B;
            ucsrc = &UCSR2C;
            udr   = &UDR2;
            ucsra = &UCSR2A;
        } else if (uartNum == 3) {
            ubrrh = &UBRR3H;
            ubrrl = &UBRR3L;
            ucsrb = &UCSR3B;
            ucsrc = &UCSR3C;
            udr   = &UDR3;
            ucsra = &UCSR3A;
        }
    #endif
}


void UART::begin(unsigned long baud, void (*callback)(const char *)) {
    data_callback = callback;
    init(baud);
}

void UART::init(unsigned long baud) {
    // Tentukan apakah mode U2X diperlukan untuk baud rate tinggi
    unsigned int ubrr;
    if (baud >= 115200) {
        // Hitung UBRR dengan pembagi 8 (mode U2X aktif)
        ubrr = (F_CPU / (8UL * baud)) - 1;
        #if defined(__AVR_ATmega8__)
        *ucsra |= (1 << U2X);  // Aktifkan U2X (bit U2X pada ATmega8)
        #else
        *ucsra |= (1 << U2X0); // Aktifkan U2X (bit U2X0 pada ATmega328P/2560)
        #endif
    } else {
        // Hitung UBRR dengan pembagi 16 (mode U2X non-aktif)
        ubrr = (F_CPU / (16UL * baud)) - 1;
        #if defined(__AVR_ATmega8__)
        *ucsra &= ~(1 << U2X); // Matikan U2X (bit U2X pada ATmega8)
        #else
        *ucsra &= ~(1 << U2X0); // Matikan U2X (bit U2X0 pada ATmega328P/2560)
        #endif
    }

    // Set register UBRR
    *ubrrh = (ubrr >> 8);
    *ubrrl = ubrr;

    // Konfigurasi register kontrol UART
    #if defined(__AVR_ATmega8__)
    *ucsrb = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);  // Aktifkan TX, RX, dan interrupt RX
    *ucsrc = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0); // Set frame format: 8 data bits, 1 stop bit
    #else
    *ucsrb = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); // Aktifkan TX, RX, dan interrupt RX
    *ucsrc = (1 << UCSZ01) | (1 << UCSZ00); // Set frame format: 8 data bits, 1 stop bit
    #endif
    sei();  // Aktifkan global interrupt
}

void UART::send(char data) {
    #if defined(__AVR_ATmega8__)
    while (!(*ucsra & (1 << UDRE)));
#else
    while (!(*ucsra & (1 << UDRE0)));
#endif
    *udr = data;
}

void UART::send(const char *str) {
    while (*str) send(*str++);
}

void UART::send(int32_t data) {
    char buf[12];
    itoa(data, buf, 10);
    send(buf);
}

void UART::send(uint32_t data) {
    char buf[12];
    utoa(data, buf, 10);
    send(buf);
}

void UART::send(uint16_t data) {
    // Overload khusus untuk uint16_t: konversi ke uint32_t kemudian panggil send(uint32_t)
    send(static_cast<uint32_t>(data));
}

void UART::send(int16_t data) {
    // Overload khusus untuk int16_t: konversi ke int32_t kemudian panggil send(int32_t)
    send(static_cast<int32_t>(data));
}

void UART::send(uint8_t data) {
    // Overload khusus untuk uint8_t: konversi ke uint32_t kemudian panggil send(uint32_t)
    send(static_cast<uint32_t>(data));
}

void UART::send(int8_t data) {
    // Overload khusus untuk int8_t: konversi ke int32_t kemudian panggil send(int32_t)
    send(static_cast<int32_t>(data));
}

void UART::send(float data) {
    char buf[10];
    dtostrf(data, 6, 2, buf);
    send(buf);
}

void UART::send(bool data) {
    // Menggunakan send_P untuk mengirim string dari PROGMEM
    send_P(data ? trueStr : falseStr);
}

// TAMBAHAN: Implementasi sendLine untuk char
void UART::sendLine(char data) {
    send(data);
    send_P(newLine);
}

void UART::sendLine(const char *str) {
    send(str);
    send_P(newLine);
}

void UART::sendLine(int32_t data) {
    send(data);
    send_P(newLine);
}

void UART::sendLine(uint32_t data) {
    send(data);
    send_P(newLine);
}

void UART::sendLine(uint16_t data) {
    // Overload khusus untuk uint16_t: konversi ke uint32_t kemudian panggil sendLine(uint32_t)
    sendLine(static_cast<uint32_t>(data));
}

void UART::sendLine(int16_t data) {
    // Overload khusus untuk int16_t: konversi ke int32_t kemudian panggil sendLine(int32_t)
    sendLine(static_cast<int32_t>(data));
}

void UART::sendLine(uint8_t data) {
    // Overload khusus untuk uint8_t: konversi ke uint32_t kemudian panggil sendLine(uint32_t)
    sendLine(static_cast<uint32_t>(data));
}

void UART::sendLine(int8_t data) {
    // Overload khusus untuk int8_t: konversi ke int32_t kemudian panggil sendLine(int32_t)
    sendLine(static_cast<int32_t>(data));
}

void UART::sendLine(float data) {
    send(data);
    send_P(newLine);
}

void UART::sendLine(bool data) {
    send(data);
    send_P(newLine);
}

void UART::receive(char received) {
    if (received == '\n' || received == '\r') {
        buffer[index] = '\0';
        if (data_callback) data_callback(buffer);
        index = 0;
    } else if (index < sizeof(buffer) - 1) {
        buffer[index++] = received;
    }
}

void UART::handleInterrupt() {
    receive(*udr);
}

// Fungsi pembantu untuk mengirim string dari PROGMEM
void UART::send_P(const char *progmem_str) {
    char c;
    while ((c = pgm_read_byte(progmem_str++))) {
        send(c);
    }
}

#if defined(__AVR_ATmega8__)
    ISR(USART_RXC_vect) { uart.handleInterrupt(); }
#else
    ISR(USART_RX_vect) { uart.handleInterrupt(); }
    #if defined(__AVR_ATmega2560__)
    ISR(USART1_RX_vect) { uart1.handleInterrupt(); }
    ISR(USART2_RX_vect) { uart2.handleInterrupt(); }
    ISR(USART3_RX_vect) { uart3.handleInterrupt(); }
    #endif
#endif

#endif