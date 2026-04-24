#include <Arduino.h>
#include <stdint.h>
#include <FastLED.h>

// LED decleration
#define NUM_LEDS 25
#define DIN_PIN 13


void leds_init();

void set_led(int idx);