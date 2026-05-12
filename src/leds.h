#include <Arduino.h>
#include <stdint.h>
#include <FastLED.h>

// LED decleration
#define NUM_LEDS 25
#define DIN1_PIN 13
#define DIN2_PIN 9
#define DIN3_PIN 10
#define DIN4_PIN 11


void leds_init();

void set_led(int idx);

void test_led();

void show_move(String move);

void clear_leds();