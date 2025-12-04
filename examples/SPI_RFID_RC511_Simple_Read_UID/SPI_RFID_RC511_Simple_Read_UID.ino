#include <ArduLite.h>
#include <ArduLiteUART.h>
#include <ArduLiteSPI.h>
#include <ArduLiteSPI_RFID.h>

ArduLiteSPI spi;
ArduLiteSPI_RFID rfid(&spi, 10, 9); //sda, rst

int main() {
  uart.begin(9600);
  spi.begin();
  rfid.begin();

  uart.sendLine("RFID RC522 Ready!");

  while (1) {
    if (rfid.detectCard()) {
      uart.sendLine("Card detected!!!");
      uart.send("UID: ");
      uart.send(rfid.uid[0]);
      uart.send(" ");
      uart.send(rfid.uid[1]);
      uart.send(" ");
      uart.send(rfid.uid[2]);
      uart.send(" ");
      uart.sendLine(rfid.uid[3]);
    }
  }

}
