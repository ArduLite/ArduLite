#include <ArduLite.h>
#include <ArduLiteTime.h>
#include <ArduLiteUART.h>

int main() {
  uart.begin(9600);
  uint32_t lastCheck = 0;

  while (1) {
    uint32_t current = time.now();

    if (current - lastCheck >= 1000) {
      uart.sendLine(current);
      lastCheck = current;
    }
  }
}
