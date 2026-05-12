#include "leds.h"

CRGB leds[NUM_LEDS];

// define all the possible tiles
const int a8[4] = {0, 1, 8, 9};
const int b8[4] = {1, 2, 7, 8};
const int c8[4] = {2, 3, 6, 7};
const int d8[4] = {3, 4, 5, 6};
const int d7[4] = {5, 6, 13, 14};
const int c7[4] = {6, 7, 12, 13};
const int b7[4] = {7, 8, 11, 12};
const int a7[4] = {8, 9, 10, 11};
const int a6[4] = {10, 11, 18, 19};
const int b6[4] = {11, 12, 17, 18};
const int c6[4] = {12, 13, 16, 17};
const int d6[4] = {13, 14, 15, 16};
const int d5[4] = {15, 16, 23, 24};
const int c5[4] = {16, 17, 22, 23};
const int b5[4] = {17, 18, 21, 22};
const int a5[4] = {18, 19, 20, 21};

void leds_init(){
  // led chain setup
  FastLED.addLeds<WS2812, DIN1_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  return;
}

void set_led(int idx){
  FastLED.clear();

  if (idx >= 0 && idx < NUM_LEDS) {
    leds[idx] = CRGB::Blue;
  }

  FastLED.show();
  return;
}

void test_led(){
  for (int i = 0; i < 25; i++){
    FastLED.clear();
    leds[i] = CRGB::Red;
    delay(200);
    FastLED.show();
  }
  return;
}

void light_tile_leds(const int tile_leds[4], CRGB color) {
  for (int i = 0; i < 4; i++) {
    int led_idx = tile_leds[i];

    if (led_idx >= 0 && led_idx < NUM_LEDS) {
      leds[led_idx] = color;
    }
  }
}

void light_tile(String tile) {
  if (tile == "a8") {
    light_tile_leds(a8, CRGB::Blue);
  }
  else if (tile == "b8") {
    light_tile_leds(b8, CRGB::Blue);
  }
  else if (tile == "c8") {
    light_tile_leds(c8, CRGB::Blue);
  }
  else if (tile == "d8") {
    light_tile_leds(d8, CRGB::Blue);
  }

  else if (tile == "a7") {
    light_tile_leds(a7, CRGB::Blue);
  }
  else if (tile == "b7") {
    light_tile_leds(b7, CRGB::Blue);
  }
  else if (tile == "c7") {
    light_tile_leds(c7, CRGB::Blue);
  }
  else if (tile == "d7") {
    light_tile_leds(d7, CRGB::Blue);
  }

  else if (tile == "a6") {
    light_tile_leds(a6, CRGB::Blue);
  }
  else if (tile == "b6") {
    light_tile_leds(b6, CRGB::Blue);
  }
  else if (tile == "c6") {
    light_tile_leds(c6, CRGB::Blue);
  }
  else if (tile == "d6") {
    light_tile_leds(d6, CRGB::Blue);
  }

  else if (tile == "a5") {
    light_tile_leds(a5, CRGB::Blue);
  }
  else if (tile == "b5") {
    light_tile_leds(b5, CRGB::Blue);
  }
  else if (tile == "c5") {
    light_tile_leds(c5, CRGB::Blue);
  }
  else if (tile == "d5") {
    light_tile_leds(d5, CRGB::Blue);
  }

  else {
    Serial.print("Unknown tile: ");
    Serial.println(tile);
  }
}

void show_move(String move){

  // combine first two to get first tile to light up
  String tile1 = move.substring(0, 2);

  String tile2 = move.substring(2, 4);
  FastLED.clear();
  light_tile(tile1);
  light_tile(tile2);
  FastLED.show();
  return;
}

void clear_leds(){
  FastLED.clear();
  FastLED.show();
  return;
}