#include <ArduLite.h>
#include <ArduLiteUART.h>

int main() {

  uart.begin(9600);
  Analog sensor(A0);
  sensor.setPrescaler(2);

  int dataAnalog = 0;

  testSpeed(1);
  dataAnalog = sensor.read();
  uint16_t value = testSpeed(0); 

  uart.send("Cycles: ");
  uart.sendLine(value - 1);
  uart.send("Microseconds: ");
  uart.sendLine((float)(value - 1) / 16);

  uart.send("Data Analog: ");
  uart.sendLine(dataAnalog);
}
