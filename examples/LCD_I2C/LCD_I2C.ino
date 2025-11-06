#include <ArduLiteI2C_LCD.h>

int main() {

  I2C_LCD lcd(0x27);
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("Hello World!");
  lcd.setCursor(0, 1);
  lcd.print("ArduLite LCD");

}
