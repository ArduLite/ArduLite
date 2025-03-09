#include <ArduLite.h>

int main(){
  PWM led(9);
  Analog sensor(A0);

  while(1){

    int sensorValue = sensor.read();
    float brightness = sensorValue * (255.0 / 1023.0);
    led.write(brightness);
    
  }
}
