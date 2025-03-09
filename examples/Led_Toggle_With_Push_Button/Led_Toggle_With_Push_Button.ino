#include <ArduLite.h>

int main(){
  Digital button(2, IN);
  Digital led(13, OUT);

  while(1){
    
    if(!button.read()){
      led.toggle();
      wait(500);
    }
    
  }
}
