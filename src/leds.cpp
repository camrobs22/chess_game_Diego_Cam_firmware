#include "leds.h"

CRGB leds[NUM_LEDS];

void leds_init(){
  // led chain setup
  FastLED.addLeds<WS2812, DIN_PIN, GRB>(leds, NUM_LEDS);
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