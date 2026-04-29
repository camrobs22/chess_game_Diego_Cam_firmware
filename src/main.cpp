#include <Arduino.h>
#include <stdint.h>
#include "hall_sensor.h"
#include "leds.h"
#include "state.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  hall_init();
  leds_init();
  setup_state();
}

void loop() {
  // put your main code here, to run repeatedly:
  test_hall();
  set_led(0);
  delay(100);
  // set a tolerance to detect change in state
  //float tol = 0.5;
  // interrupt that sets every __ ms
  if (ready_for_state_update()){
    update_state();
    // check to see if anything has changed
    bool same = true;

    for (int r = 0; r < 8; r++) {
      for (int c = 0; c < 8; c++) {
        // if (GameState.cur_state[r][c] > GameState.past_state[r][c] + tol || GameState.cur_state[r][c] < GameState.past_state[r][c] - tol) {
        // might need to change to adc volt values if this doesn't work and have a statement like above
        if (GameState.cur_state[r][c] != GameState.past_state[r][c]){ 
          same = false;
        }
      }
    }
  }
}
