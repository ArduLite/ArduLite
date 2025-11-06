/*
    Created by Ajang Rahmat (email: ajangrahmat@gmail.com),
    Kelas Robot PT AViSha Inovasi Indonesia.
*/

#ifndef ARDULITE_H
#define ARDULITE_H

#define int8 int8_t
#define int16 int16_t
#define int32 int32_t
#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t

#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t

#include <util/delay.h>
#define wait(x) _delay_ms(x)
#define waitMicro(x) _delay_us(x)
#define waitMicroSeconds(x) _delay_us(x)
#define activeLOW ACTIVE_OFF
#define activeHIGH ACTIVE_ON
#define ACTIVE_HIGH ACTIVE_ON
#define ACTIVE_LOW ACTIVE_OFF
#define activeON ACTIVE_ON
#define activeOFF ACTIVE_OFF

#include "ArduLiteDigital.h"
#include "ArduLiteAnalog.h"
#include "ArduLitePWM.h"
#include "ArduLiteUtils.h"

#endif // ARDULITE_H