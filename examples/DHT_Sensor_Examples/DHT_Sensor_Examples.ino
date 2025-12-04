#include <ArduLite.h>
#include <ArduLiteDHT.h>
#include <ArduLiteUART.h>

int main() {
    DHT dht(4, 11);
    uart.begin(9600);

    while (1) {
        float temperature, humidity;
        if (dht.readBoth(temperature, humidity)) {
            uart.send(temperature);
            uart.send(" ");
            uart.sendLine(humidity); 
        } else {
            uart.sendLine("Read failed, check the circuit!");
        }
        wait(500);
    }
}
