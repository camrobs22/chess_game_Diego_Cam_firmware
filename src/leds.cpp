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
    leds[idx] = CRGB::Blue;
    return;
}