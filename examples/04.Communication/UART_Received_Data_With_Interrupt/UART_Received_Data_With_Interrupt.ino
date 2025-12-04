#include <ArduLite.h>
#include <ArduLiteUART.h>
Digital led(13, OUT);

void onDataReceived(const char *data) {
  uart.sendLine("You typed:");
  uart.sendLine(data);

  if (strcmp(data, "ON") == 0) {
    led.on();
    uart.sendLine("LED turned on!");
  } else if (strcmp(data, "OFF") == 0) {
    led.off();
    uart.sendLine("LED turned off!");
  }
}

int main() {
  uart.begin(9600, onDataReceived);
  uart.sendLine("ArduLite UART Ready!");

  while (1) {
    uart.sendLine("Waiting for command...");
    delay(5000);
  }
}
