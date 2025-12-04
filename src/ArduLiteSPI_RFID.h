// ArduLiteSPI_RFID.h
#ifndef ARDULITE_SPI_RFID_H
#define ARDULITE_SPI_RFID_H

#include "ArduLiteSPI.h"

// RC522 Commands
#define PCD_IDLE 0x00
#define PCD_TRANSCEIVE 0x0C
#define PCD_RESETPHASE 0x0F

// RC522 Registers
#define CommandReg 0x01
#define ComIEnReg 0x02
#define ComIrqReg 0x04
#define ErrorReg 0x06
#define FIFODataReg 0x09
#define FIFOLevelReg 0x0A
#define ControlReg 0x0C
#define BitFramingReg 0x0D
#define ModeReg 0x11
#define TxControlReg 0x14
#define TxASKReg 0x15
#define TModeReg 0x2A
#define TPrescalerReg 0x2B
#define TReloadRegL 0x2D
#define TReloadRegH 0x2C

// PICC Commands
#define PICC_REQIDL 0x26
#define PICC_ANTICOLL 0x93

// Status
#define MI_OK 0
#define MI_NOTAGERR 1
#define MI_ERR 2

class ArduLiteSPI_RFID {
private:
    ArduLiteSPI* spi;
    unsigned char ssPin;
    unsigned char rstPin;
    
    void writeRegister(unsigned char reg, unsigned char value) {
        spi->selectDevice(ssPin);
        spi->transfer((reg << 1) & 0x7E);
        spi->transfer(value);
        spi->deselectDevice(ssPin);
    }
    
    unsigned char readRegister(unsigned char reg) {
        unsigned char value;
        spi->selectDevice(ssPin);
        spi->transfer(((reg << 1) & 0x7E) | 0x80);
        value = spi->transfer(0x00);
        spi->deselectDevice(ssPin);
        return value;
    }
    
    void setBitMask(unsigned char reg, unsigned char mask) {
        unsigned char tmp = readRegister(reg);
        writeRegister(reg, tmp | mask);
    }
    
    void clearBitMask(unsigned char reg, unsigned char mask) {
        unsigned char tmp = readRegister(reg);
        writeRegister(reg, tmp & (~mask));
    }
    
    void antennaOn() {
        unsigned char temp = readRegister(TxControlReg);
        if (!(temp & 0x03)) {
            setBitMask(TxControlReg, 0x03);
        }
    }
    
    unsigned char toCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned int *backLen) {
        unsigned char status = MI_ERR;
        unsigned char irqEn = 0x00;
        unsigned char waitIRq = 0x00;
        unsigned char n;
        unsigned int i;
        
        if (command == PCD_TRANSCEIVE) {
            irqEn = 0x77;
            waitIRq = 0x30;
        }
        
        writeRegister(ComIEnReg, irqEn | 0x80);
        clearBitMask(ComIrqReg, 0x80);
        setBitMask(FIFOLevelReg, 0x80);
        writeRegister(CommandReg, PCD_IDLE);
        
        for (i = 0; i < sendLen; i++) {
            writeRegister(FIFODataReg, sendData[i]);
        }
        
        writeRegister(CommandReg, command);
        if (command == PCD_TRANSCEIVE) {
            setBitMask(BitFramingReg, 0x80);
        }
        
        i = 2000;
        do {
            n = readRegister(ComIrqReg);
            i--;
        } while ((i != 0) && !(n & 0x01) && !(n & waitIRq));
        
        clearBitMask(BitFramingReg, 0x80);
        
        if (i != 0) {
            if (!(readRegister(ErrorReg) & 0x1B)) {
                status = MI_OK;
                
                if (n & irqEn & 0x01) {
                    status = MI_NOTAGERR;
                }
                
                if (command == PCD_TRANSCEIVE) {
                    n = readRegister(FIFOLevelReg);
                    unsigned char lastBits = readRegister(ControlReg) & 0x07;
                    
                    if (lastBits) {
                        *backLen = (n - 1) * 8 + lastBits;
                    } else {
                        *backLen = n * 8;
                    }
                    
                    if (n == 0) n = 1;
                    if (n > 16) n = 16;
                    
                    for (i = 0; i < n; i++) {
                        backData[i] = readRegister(FIFODataReg);
                    }
                }
            }
        }
        
        return status;
    }

public:
    unsigned char uid[5];
    
    // Constructor
    ArduLiteSPI_RFID(ArduLiteSPI* spiInstance, unsigned char ss, unsigned char rst) {
        spi = spiInstance;
        ssPin = ss;
        rstPin = rst;
    }
    
    // Inisialisasi RFID
    void begin() {
        spi->setCS(ssPin);
        
        // Set RST pin sebagai output
        if (rstPin >= 8 && rstPin <= 13) {
            DDRB |= (1 << (rstPin - 8));
            PORTB &= ~(1 << (rstPin - 8));
            _delay_ms(10);
            PORTB |= (1 << (rstPin - 8));
        } else if (rstPin >= 0 && rstPin <= 7) {
            DDRD |= (1 << rstPin);
            PORTD &= ~(1 << rstPin);
            _delay_ms(10);
            PORTD |= (1 << rstPin);
        }
        
        _delay_ms(50);
        
        // Reset RC522
        writeRegister(CommandReg, PCD_RESETPHASE);
        
        writeRegister(TModeReg, 0x8D);
        writeRegister(TPrescalerReg, 0x3E);
        writeRegister(TReloadRegL, 30);
        writeRegister(TReloadRegH, 0);
        writeRegister(TxASKReg, 0x40);
        writeRegister(ModeReg, 0x3D);
        
        antennaOn();
    }
    
    // Request kartu
    unsigned char request(unsigned char reqMode, unsigned char *TagType) {
        unsigned char status;
        unsigned int backBits;
        
        writeRegister(BitFramingReg, 0x07);
        
        TagType[0] = reqMode;
        status = toCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
        
        if ((status != MI_OK) || (backBits != 0x10)) {
            status = MI_ERR;
        }
        
        return status;
    }
    
    // Anticollision
    unsigned char anticoll(unsigned char *serNum) {
        unsigned char status;
        unsigned char i;
        unsigned char serNumCheck = 0;
        unsigned int unLen;
        
        writeRegister(BitFramingReg, 0x00);
        
        serNum[0] = PICC_ANTICOLL;
        serNum[1] = 0x20;
        
        status = toCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
        
        if (status == MI_OK) {
            for (i = 0; i < 4; i++) {
                serNumCheck ^= serNum[i];
            }
            if (serNumCheck != serNum[i]) {
                status = MI_ERR;
            }
        }
        
        return status;
    }
    
    // Deteksi kartu dan baca UID
    bool detectCard() {
        unsigned char status;
        unsigned char str[16];
        
        status = request(PICC_REQIDL, str);
        
        if (status == MI_OK) {
            status = anticoll(uid);
            if (status == MI_OK) {
                return true;
            }
        }
        return false;
    }
    
    // Get UID sebagai array
    void getUID(unsigned char* buffer) {
        for (unsigned char i = 0; i < 4; i++) {
            buffer[i] = uid[i];
        }
    }
};

#endif // ARDULITE_SPI_RFID_H