#include <ArduLite.h>

int main() {
  Digital led(13, OUT, ACTIVE_LOW);

  loop() {
    led.on();
    wait(1000);
    led.off();
    wait(1000);
  }
}
