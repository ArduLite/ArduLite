#include <ArduLite.h>
#include <ArduLiteUART.h>
#include <ArduLitePulse.h>

int main() {
  uart.begin(9600);
  Pulse echo(12, IN);
  
  while (1) {
    unsigned long duration = echo.read();
    uart.sendLine(duration);
    wait(100);
  }
  
}
