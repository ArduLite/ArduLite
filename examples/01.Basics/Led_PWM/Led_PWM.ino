#include <ArduLite.h>

int main(){
  PWM led(9);

  while(1){
    
    for(byte i = 0; i < 255; i++){
      led.write(i);
      wait(5);
    }

    for(byte i = 255; i > 0; i--){
      led.write(i);
      wait(5);
    }
    
  }
}
