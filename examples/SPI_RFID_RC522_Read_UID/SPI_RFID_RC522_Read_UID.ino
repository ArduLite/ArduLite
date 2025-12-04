#include "ArduLite.h"
#include "ArduLiteSPI.h"
#include "ArduLiteSPI_RFID.h"
#include "ArduLiteUART.h"

ArduLiteSPI spi;
ArduLiteSPI_RFID rfid(&spi, 10, 9);  // SS = Pin 10, RST = Pin 9

int main(void) {
  // Setup - run once
  uart.begin(9600);
  spi.begin();
  rfid.begin();

  uart.sendLine("Ready!");

  // Loop - run continuously
  while (1) {

    // Check if a card is present
    if (rfid.detectCard()) {

      uart.sendLine("Card detected!");

      // Check a specific card UID (using normal decimal numbers)
      // Example UID: 18 52 86 120
      if (rfid.uid[0] == 83 &&
          rfid.uid[1] == 66 &&
          rfid.uid[2] == 214 &&
          rfid.uid[3] == 15) {

        uart.sendLine("ADMIN Card - Access Granted!");

      }
      // Check another card
      // Example UID: 170 187 204 221
      else if (rfid.uid[0] == 170 &&
               rfid.uid[1] == 187 &&
               rfid.uid[2] == 204 &&
               rfid.uid[3] == 221) {

        uart.sendLine("USER Card - Limited Access!");

      }
      // Unknown card
      else {
        uart.sendLine("UNKNOWN Card - Access Denied!");
      }

      // Display UID
      uart.send("UID: ");
      for (int i = 0; i < 4; i++) {
        uart.send(rfid.uid[i]);
        uart.send(" ");
      }
      uart.sendLine("");

      wait(2000);  // Wait 2 seconds
    }

    wait(100);  // Wait 0.1 second
  }

  return 0;
}
