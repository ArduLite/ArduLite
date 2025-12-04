#ifndef ARDULITE_KEYPAD_H
#define ARDULITE_KEYPAD_H

#include <avr/io.h>
#include "ArduLiteTime.h"  // Include time library

// Key event types
enum KeyEventType {
    KEY_PRESSED,
    KEY_RELEASED,
    KEY_HELD
};

// Key event structure
struct KeyEvent {
    char key;
    KeyEventType type;
    uint16_t duration; // in milliseconds
};

// Key state for each key
struct KeyState {
    char key;
    bool pressed;
    uint32_t pressTime;
    bool reported;
};

class Keypad {
  private:
    uint8_t _rows, _cols;
    uint8_t _rowPins[4], _colPins[4];
    char _keys[4][4];
    KeyState _keyStates[16];
    uint8_t _keyCount;
    
    // Event callback function pointer
    void (*_eventCallback)(KeyEvent event);
    
    // Debounce settings
    uint16_t _debounceMs;
    uint16_t _holdThresholdMs;

    // Pin control methods
    void writePin(uint8_t pin, bool state) {
        volatile uint8_t* port;
        uint8_t mask;
        
        if (pin <= 7) { port = &PORTD; mask = (1 << pin); }
        else if (pin <= 13) { port = &PORTB; mask = (1 << (pin - 8)); }
        else { port = &PORTC; mask = (1 << (pin - 14)); }
        
        if (state) *port |= mask;
        else *port &= ~mask;
    }
    
    bool readPin(uint8_t pin) {
        volatile uint8_t* port;
        uint8_t mask;
        
        if (pin <= 7) { port = &PIND; mask = (1 << pin); }
        else if (pin <= 13) { port = &PINB; mask = (1 << (pin - 8)); }
        else { port = &PINC; mask = (1 << (pin - 14)); }
        
        return (*port & mask);
    }
    
    void setPinOutput(uint8_t pin) {
        volatile uint8_t* ddr;
        
        if (pin <= 7) ddr = &DDRD;
        else if (pin <= 13) ddr = &DDRB;
        else ddr = &DDRC;
        
        uint8_t bit = (pin <= 7 ? pin : (pin <= 13 ? pin - 8 : pin - 14));
        *ddr |= (1 << bit);
    }
    
    void setPinInput(uint8_t pin, bool pullup = true) {
        volatile uint8_t* ddr, *port;
        
        if (pin <= 7) { ddr = &DDRD; port = &PORTD; }
        else if (pin <= 13) { ddr = &DDRB; port = &PORTB; }
        else { ddr = &DDRC; port = &PORTC; }
        
        uint8_t bit = (pin <= 7 ? pin : (pin <= 13 ? pin - 8 : pin - 14));
        *ddr &= ~(1 << bit);
        if (pullup) *port |= (1 << bit);
    }

    void setupDefaultKeymap() {
        if(_rows == 4 && _cols == 4) {
            char keys[4][4] = {
                {'1', '2', '3', 'A'},
                {'4', '5', '6', 'B'}, 
                {'7', '8', '9', 'C'},
                {'*', '0', '#', 'D'}
            };
            for(uint8_t i=0; i<4; i++) 
                for(uint8_t j=0; j<4; j++) 
                    _keys[i][j] = keys[i][j];
        }
        else if(_rows == 4 && _cols == 3) {
            char keys[4][4] = {
                {'1', '2', '3'},
                {'4', '5', '6'}, 
                {'7', '8', '9'},
                {'*', '0', '#'}
            };
            for(uint8_t i=0; i<4; i++) 
                for(uint8_t j=0; j<3; j++) 
                    _keys[i][j] = keys[i][j];
        }
        else if(_rows == 3 && _cols == 4) {
            char keys[4][4] = {
                {'1', '2', '3', 'U'},
                {'4', '5', '6', 'E'},
                {'7', '8', '9', 'D'}
            };
            for(uint8_t i=0; i<3; i++) 
                for(uint8_t j=0; j<4; j++) 
                    _keys[i][j] = keys[i][j];
        }
        else if(_rows == 3 && _cols == 3) {
            char keys[4][4] = {
                {'1', '2', '3'},
                {'4', '5', '6'}, 
                {'7', '8', '9'}
            };
            for(uint8_t i=0; i<3; i++) 
                for(uint8_t j=0; j<3; j++) 
                    _keys[i][j] = keys[i][j];
        }
        else if(_rows == 2 && _cols == 2) {
            char keys[4][4] = {
                {'U', 'R'},
                {'L', 'D'}
            };
            for(uint8_t i=0; i<2; i++) 
                for(uint8_t j=0; j<2; j++) 
                    _keys[i][j] = keys[i][j];
        }
        
        // Initialize key states
        _keyCount = 0;
        for(uint8_t i=0; i<_rows; i++) {
            for(uint8_t j=0; j<_cols; j++) {
                _keyStates[_keyCount].key = _keys[i][j];
                _keyStates[_keyCount].pressed = false;
                _keyStates[_keyCount].pressTime = 0;
                _keyStates[_keyCount].reported = false;
                _keyCount++;
            }
        }
    }
    
    KeyState* findKeyState(char key) {
        for(uint8_t i=0; i<_keyCount; i++) {
            if(_keyStates[i].key == key) {
                return &_keyStates[i];
            }
        }
        return nullptr;
    }

  public:
    // ===== CONSTRUCTORS =====
    
    Keypad() : _rows(0), _cols(0), _eventCallback(nullptr), 
               _debounceMs(20), _holdThresholdMs(1000) {}
    
    // 4x4 Keypad
    Keypad(uint8_t r0, uint8_t r1, uint8_t r2, uint8_t r3, 
           uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3) {
        _rows = 4; _cols = 4;
        _rowPins[0] = r0; _rowPins[1] = r1; _rowPins[2] = r2; _rowPins[3] = r3;
        _colPins[0] = c0; _colPins[1] = c1; _colPins[2] = c2; _colPins[3] = c3;
        _eventCallback = nullptr;
        _debounceMs = 20;
        _holdThresholdMs = 1000;
        setupDefaultKeymap();
    }
    
    // Custom size keypad
    Keypad(uint8_t rows, uint8_t cols, uint8_t* rowPins, uint8_t* colPins) {
        _rows = rows; _cols = cols;
        for(uint8_t i=0; i<rows; i++) _rowPins[i] = rowPins[i];
        for(uint8_t i=0; i<cols; i++) _colPins[i] = colPins[i];
        _eventCallback = nullptr;
        _debounceMs = 20;
        _holdThresholdMs = 1000;
        setupDefaultKeymap();
    }
    
    // 3x3 Keypad
    Keypad(uint8_t r0, uint8_t r1, uint8_t r2,
           uint8_t c0, uint8_t c1, uint8_t c2) {
        _rows = 3; _cols = 3;
        _rowPins[0] = r0; _rowPins[1] = r1; _rowPins[2] = r2;
        _colPins[0] = c0; _colPins[1] = c1; _colPins[2] = c2;
        _eventCallback = nullptr;
        _debounceMs = 20;
        _holdThresholdMs = 1000;
        setupDefaultKeymap();
    }
    
    // 2x2 Keypad
    Keypad(uint8_t r0, uint8_t r1, uint8_t c0, uint8_t c1) {
        _rows = 2; _cols = 2;
        _rowPins[0] = r0; _rowPins[1] = r1;
        _colPins[0] = c0; _colPins[1] = c1;
        _eventCallback = nullptr;
        _debounceMs = 20;
        _holdThresholdMs = 1000;
        setupDefaultKeymap();
    }

    // ===== INITIALIZATION =====
    void begin() {
        // Setup rows as outputs
        for(uint8_t i=0; i<_rows; i++) {
            setPinOutput(_rowPins[i]);
            writePin(_rowPins[i], HIGH);
        }
        
        // Setup columns as inputs with pull-up
        for(uint8_t i=0; i<_cols; i++) {
            setPinInput(_colPins[i], true);
        }
    }
    
    // ===== FLEXIBLE KEYMAP METHODS =====
    
    template<size_t ROWS, size_t COLS>
    void setFlexibleKeymap(const char (&keys)[ROWS][COLS]) {
        // Reset keymap
        for(uint8_t i=0; i<4; i++) {
            for(uint8_t j=0; j<4; j++) {
                _keys[i][j] = '\0';
            }
        }
        
        // Copy dari keymap fleksibel ke full keymap
        for(uint8_t i=0; i<ROWS && i<4; i++) {
            for(uint8_t j=0; j<COLS && j<4; j++) {
                _keys[i][j] = keys[i][j];
            }
        }
        
        // Update key states
        _keyCount = 0;
        for(uint8_t i=0; i<_rows; i++) {
            for(uint8_t j=0; j<_cols; j++) {
                if(_keyCount < 16) {
                    _keyStates[_keyCount].key = _keys[i][j];
                    _keyStates[_keyCount].pressed = false;
                    _keyStates[_keyCount].pressTime = 0;
                    _keyStates[_keyCount].reported = false;
                    _keyCount++;
                }
            }
        }
    }
    
    // Method untuk array 1D
    void setFlexibleKeymap(const char* keys, uint8_t rows, uint8_t cols) {
        // Reset keymap
        for(uint8_t i=0; i<4; i++) {
            for(uint8_t j=0; j<4; j++) {
                _keys[i][j] = '\0';
            }
        }
        
        // Copy dari array 1D
        for(uint8_t i=0; i<rows && i<4; i++) {
            for(uint8_t j=0; j<cols && j<4; j++) {
                _keys[i][j] = keys[i * cols + j];
            }
        }
        
        // Update key states
        _keyCount = 0;
        for(uint8_t i=0; i<_rows; i++) {
            for(uint8_t j=0; j<_cols; j++) {
                if(_keyCount < 16) {
                    _keyStates[_keyCount].key = _keys[i][j];
                    _keyStates[_keyCount].pressed = false;
                    _keyStates[_keyCount].pressTime = 0;
                    _keyStates[_keyCount].reported = false;
                    _keyCount++;
                }
            }
        }
    }
    
    // ===== EVENT-BASED METHODS =====
    
    void setEventListener(void (*callback)(KeyEvent event)) {
        _eventCallback = callback;
    }
    
    void setDebounceTime(uint16_t ms) {
        _debounceMs = ms;
    }
    
    void setHoldThreshold(uint16_t ms) {
        _holdThresholdMs = ms;
    }
    
    void update() {
        static uint32_t lastScanTime = 0;
        uint32_t currentTime = time.millis(); // Gunakan time dari ArduLiteTime
        
        // Limit scan rate
        if(currentTime - lastScanTime < 10) return;
        lastScanTime = currentTime;
        
        for(uint8_t r=0; r<_rows; r++) {
            // Activate current row
            for(uint8_t i=0; i<_rows; i++) {
                writePin(_rowPins[i], (i != r));
            }
            
            time.delayMicros(100); // Gunakan delay dari ArduLiteTime
            
            // Scan columns
            for(uint8_t c=0; c<_cols; c++) {
                char key = _keys[r][c];
                KeyState* keyState = findKeyState(key);
                
                if(!keyState) continue;
                
                bool pressed = !readPin(_colPins[c]);
                
                if(pressed && !keyState->pressed) {
                    // Key pressed
                    keyState->pressed = true;
                    keyState->pressTime = currentTime;
                    keyState->reported = false;
                    
                    if(_eventCallback) {
                        KeyEvent event;
                        event.key = key;
                        event.type = KEY_PRESSED;
                        event.duration = 0;
                        _eventCallback(event);
                    }
                }
                else if(pressed && keyState->pressed) {
                    // Key held
                    uint16_t duration = currentTime - keyState->pressTime;
                    
                    if(duration >= _holdThresholdMs && !keyState->reported) {
                        keyState->reported = true;
                        
                        if(_eventCallback) {
                            KeyEvent event;
                            event.key = key;
                            event.type = KEY_HELD;
                            event.duration = duration;
                            _eventCallback(event);
                        }
                    }
                }
                else if(!pressed && keyState->pressed) {
                    // Key released
                    uint16_t duration = currentTime - keyState->pressTime;
                    keyState->pressed = false;
                    
                    if(_eventCallback) {
                        KeyEvent event;
                        event.key = key;
                        event.type = KEY_RELEASED;
                        event.duration = duration;
                        _eventCallback(event);
                    }
                }
            }
        }
    }
    
    // ===== CUSTOM KEYMAP (Original) =====
    void setKeymap(const char newKeys[4][4]) {
        for(uint8_t i=0; i<_rows; i++)
            for(uint8_t j=0; j<_cols; j++)
                _keys[i][j] = newKeys[i][j];
        
        // Update key states
        _keyCount = 0;
        for(uint8_t i=0; i<_rows; i++) {
            for(uint8_t j=0; j<_cols; j++) {
                _keyStates[_keyCount].key = _keys[i][j];
                _keyStates[_keyCount].pressed = false;
                _keyStates[_keyCount].pressTime = 0;
                _keyStates[_keyCount].reported = false;
                _keyCount++;
            }
        }
    }
    
    void setSingleKey(uint8_t row, uint8_t col, char key) {
        if(row < _rows && col < _cols) {
            _keys[row][col] = key;
        }
    }
    
    // ===== TRADITIONAL POLLING METHODS =====
    char getKey() {
        for(uint8_t r=0; r<_rows; r++) {
            for(uint8_t i=0; i<_rows; i++) {
                writePin(_rowPins[i], (i != r));
            }
            
            time.delayMicros(100); // Gunakan delay dari ArduLiteTime
            
            for(uint8_t c=0; c<_cols; c++) {
                if(!readPin(_colPins[c])) {
                    time.wait(_debounceMs); // Gunakan wait dari ArduLiteTime
                    
                    // Wait for release with timeout
                    uint16_t timeout = 0;
                    while(!readPin(_colPins[c]) && timeout < 1000) { 
                        time.delayMicros(100);
                        timeout++;
                    }
                    
                    time.wait(_debounceMs);
                    return _keys[r][c];
                }
            }
        }
        return '\0';
    }
    
    // ===== UTILITY METHODS =====
    uint8_t getRows() { return _rows; }
    uint8_t getCols() { return _cols; }
    
    bool isPressed(char key) {
        KeyState* keyState = findKeyState(key);
        return keyState ? keyState->pressed : false;
    }
    
    // Time utility methods using ArduLiteTime
    static uint32_t millis() {
        return time.millis();
    }
    
    static uint32_t micros() {
        return time.micros();
    }
    
    static void delay(uint32_t ms) {
        time.wait(ms);
    }
    
    static void delayMicroseconds(uint16_t us) {
        time.delayMicros(us);
    }
};

#endif