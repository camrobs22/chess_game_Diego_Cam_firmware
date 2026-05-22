#include <Arduino.h>
#include <stdint.h>
#include <FastLED.h>

// LED decleration
#define NUM_LEDS 25
#define DIN1_PIN 13
#define DIN3_PIN 19
#define DIN2_PIN 18
#define DIN4_PIN 17


void leds_init();

bool get_tile_location(String tile, int &boardIndex, int &localRow, int &localCol);

void set_led(int idx);

void test_led();

void show_move(String move);

void clear_leds();