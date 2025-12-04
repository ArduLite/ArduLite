#ifndef ARDULITE_PASSWORD_H
#define ARDULITE_PASSWORD_H

#include <avr/io.h>
#include <string.h>

class Password {
  private:
    char _currentPassword[17];
    char _inputBuffer[17];
    uint8_t _maxLength;
    uint8_t _inputIndex;
    char _maskChar;
    
    // Callback functions
    void (*_onSuccess)();
    void (*_onFailure)();
    void (*_onInput)(const char* masked);

  public:
    // Constructor
    Password(uint8_t maxLength = 16) {
      _maxLength = maxLength;
      _inputIndex = 0;
      _maskChar = '*';
      
      _onSuccess = nullptr;
      _onFailure = nullptr;
      _onInput = nullptr;
      
      strcpy(_currentPassword, "1234"); // Default password
      clearInput();
    }
    
    // Set password baru
    void setPassword(const char* newPassword) {
      strncpy(_currentPassword, newPassword, _maxLength);
      _currentPassword[_maxLength] = '\0';
    }
    
    // Set mask character
    void setMaskChar(char mask) {
      _maskChar = mask;
    }
    
    // Set callback functions
    void onSuccess(void (*callback)()) {
      _onSuccess = callback;
    }
    
    void onFailure(void (*callback)()) {
      _onFailure = callback;
    }
    
    void onInput(void (*callback)(const char* masked)) {
      _onInput = callback;
    }
    
    // Clear input buffer
    void clearInput() {
      memset(_inputBuffer, 0, sizeof(_inputBuffer));
      _inputIndex = 0;
    }
    
    // Process key input
    bool processKey(char key) {
      if (key) {
        if (key == '#') {
          // Submit password
          bool result = check();
          if (result && _onSuccess) _onSuccess();
          if (!result && _onFailure) _onFailure();
          clearInput();
          return true;
        }
        else if (key == '*') {
          // Clear input
          clearInput();
          if (_onInput) {
            char masked[17];
            getMaskedInput(masked);
            _onInput(masked);
          }
        }
        else if (_inputIndex < _maxLength) {
          // Add character to input buffer
          _inputBuffer[_inputIndex++] = key;
          _inputBuffer[_inputIndex] = '\0';
          
          if (_onInput) {
            char masked[17];
            getMaskedInput(masked);
            _onInput(masked);
          }
        }
      }
      return false;
    }
    
    // Check if input matches password
    bool check() {
      return (strcmp(_inputBuffer, _currentPassword) == 0);
    }
    
    // Get input buffer (with mask for display)
    void getMaskedInput(char* buffer) {
      for (uint8_t i = 0; i < _inputIndex; i++) {
        buffer[i] = _maskChar;
      }
      buffer[_inputIndex] = '\0';
    }
    
    // Get raw input (without mask)
    const char* getRawInput() {
      return _inputBuffer;
    }
    
    // Check if input is empty
    bool isEmpty() {
      return (_inputIndex == 0);
    }
    
    // Get input length
    uint8_t getInputLength() {
      return _inputIndex;
    }
};

#endif