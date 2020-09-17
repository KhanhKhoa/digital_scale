#ifndef __I2C_EXTENDER_H__
#define __I2C_EXTENDER_H__

#include <Wire.h>
#include "SC16IS750.h"
#include <string.h>
#include <SPI.h>

extern SC16IS750 i2cuart;

#define BUZZER_PIN      1
#define LED_PIN         0

void setupPin();

void turnOnLED(uint8_t led_pin, uint8_t delay_time);

void turnOnBuzzerWithTime(uint8_t delay_time);

void turnOnBuzzer();

void turnOffBuzzer();

#endif