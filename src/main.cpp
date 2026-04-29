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
    // rememebr pos of change
    int col_change;
    int row_change;

    for (int r = 0; r < 8; r++) {
      for (int c = 0; c < 8; c++) {
        // if (GameState.cur_state[r][c] > GameState.past_state[r][c] + tol || GameState.cur_state[r][c] < GameState.past_state[r][c] - tol) {
        // might need to change to adc volt values if this doesn't work and have a statement like above
        if (GameState.cur_state[r][c].piecetype != GameState.past_state[r][c].piecetype){ 
          same = false;
          col_change = c;
          row_change = r;
        }
      }
    }
    // wait for sure for update then send move over to website
    if (!same){
      // make sure a move has been made count to 
      while(!valid_game_update()){
        // keep waiting and updating the state until you reach a valid game state that's different from the start
        update_state();
      }
      // send the new state over wifi to web server
      // wifi_send_state(GameState);
      // wait for response

    }
  }
}
