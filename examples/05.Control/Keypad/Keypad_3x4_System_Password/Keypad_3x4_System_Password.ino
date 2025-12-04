#include <ArduLite.h>
#include <ArduLiteUART.h>
#include <ArduLiteKeypad.h>
#include <ArduLitePassword.h>

Keypad keypad(9, 8, 7, 6, 5, 4, 3);
Password password;

void onSuccess() {
  uart.sendLine("=== CORRECT PASSWORD! ===");
  uart.sendLine("Access granted...");
}

void onFailure() {
  uart.sendLine("=== WRONG PASSWORD! ===");
  uart.send("Input: ");
  uart.sendLine(password.getRawInput());
}

void onInput(const char* masked) {
  uart.send("Password: ");
  uart.sendLine(masked);
}

int main() {
  uart.begin(9600);
  keypad.begin(4, 3); //4 rows 3 columns

  password.setPassword("1234");
  password.setMaskChar('*');
  password.onSuccess(onSuccess);
  password.onFailure(onFailure);
  password.onInput(onInput);

  uart.sendLine("=== PASSWORD SYSTEM ===");
  uart.sendLine("Password: 1234");
  uart.sendLine("Press # to submit, * to clear");
  uart.sendLine("=========================");

  while (1) {

    char key = keypad.getKey();
    if (password.processKey(key)) {
      uart.sendLine("--- Enter a new password ---");
    }

    wait(50);
  }
}
