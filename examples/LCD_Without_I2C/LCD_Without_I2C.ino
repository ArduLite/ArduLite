#include <ArduLiteLCD.h>

int main() {
  LCD lcd(12, 11, 10, 9, 8, 7);
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("PWM Backlight");
  lcd.setCursor(0, 1);
  lcd.print("0->255 Bright");
}
