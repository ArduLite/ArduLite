#include <ArduLite.h>

int main() {
  Digital led(13, OUT, ACTIVE_LOW);

  while (1) {
    led.on();
    wait(1000);
    led.off();
    wait(1000);
  }
}
