#include <ArduLite.h>
#include <ArduLiteUART.h>
#include <ArduLitePulse.h>

int main() {
  uart.begin(9600);
  Pulse echo(12, IN);
  Digital trig(11, OUT);

  loop() {

    trig.pulse(2, 10);
    unsigned long duration = echo.read();
    float distance = duration * 0.034002 / 2;

    uart.send("Distance: ");
    uart.send(distance);
    uart.sendLine(" cm");

    wait(100);
  }

}
