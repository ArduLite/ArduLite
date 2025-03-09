# ArduLite

## Overview
ArduLite is a lightweight library designed to provide essential functionalities for Arduino with minimal footprint. It is ideal for resource-constrained projects where efficiency and simplicity are crucial.

## Features
- Digital and analog pin handling
- PWM support
- Simplified syntax for ease of use
- Optimized for minimal memory usage

## Installation
1. Download the latest release from [GitHub](https://github.com/ArduLite/ArduLite).
2. Extract the folder and place it inside the Arduino `libraries` directory.
3. Restart the Arduino IDE.
4. Include the library in your project:

   ```cpp
   #include <ArduLite.h>
   ```

## Usage
### Basic Digital Control
```cpp
#include <ArduLite.h>

int main() {
    Digital button(2, IN);
    Digital led(13, OUT);

    while (1) {
        if (!button.read()) {
            led.toggle();
            wait(500);
        }
    }
}
```

### PWM Control
```cpp
#include <ArduLite.h>

int main() {
    PWM led(9);

    while (1) {
        for (byte i = 0; i < 255; i++) {
            led.write(i);
            wait(5);
        }
        for (byte i = 255; i > 0; i--) {
            led.write(i);
            wait(5);
        }
    }
}
```

### Analog Sensor Reading
```cpp
#include <ArduLite.h>

int main() {
    PWM led(9);
    Analog sensor(A0);

    while (1) {
        int sensorValue = sensor.read();
        float brightness = sensorValue * (255.0 / 1023.0);
        led.write(brightness);
    }
}
```


## Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your improvements.

## Author
**Ajang Rahmat**  
[GitHub](https://github.com/ArduLite)  
[Email](mailto:ajangrahmat@mail.com)
