#include <Arduino.h>
#include <stdint.h>
#include "hall_sensor.h"
#include "leds.h"
#include "state.h"
//#include "wifi.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("ESP32 booted successfully");
  hall_init();
  leds_init();
  setup_state();
  //print_board_state();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("In loop");
  // test_hall();
  // // set_led(4);
  // test_led();
  // delay(100);
  
  // set a tolerance to detect change in state
  //float tol = 0.5;
  // interrupt that sets every __ ms
  int count = 0;
  if (ready_for_state_update()){
    update_state();

    if (!board_scan_complete()){
      return;
    }
    // check to see if anything has changed
    bool same = true;
    // rememebr pos of change
    // int col_change;
    // int row_change;

    // int col_change2;
    // int row_change2;

    // // int start_col;
    // // int start_row;
    // // int end_col;
    // // int end_row;

    // for (int r = 0; r < 8; r++) {
    //   for (int c = 0; c < 8; c++) {
    //     // if (GameState.cur_state[r][c] > GameState.past_state[r][c] + tol || GameState.cur_state[r][c] < GameState.past_state[r][c] - tol) {
    //     // might need to change to adc volt values if this doesn't work and have a statement like above
    //     // need to change this its not detecting a change
    //     if (GameState.cur_state[r][c].piecetype != GameState.past_state[r][c].piecetype){ 
    //       same = false;
    //       // means we have already detected a change
    //       if (count > 0){
    //         if (c != col_change || r != row_change){
    //           col_change2 = c;
    //           row_change2 = r;
    //           count = 0;
    //         }
    //       }
    //       else{
    //         col_change = c;
    //         row_change = r;
    //       }

    //       Serial.printf("inside false statement row of change and col of change %i %i\r\n", r, c);
    //       float volts[8];
    //       get_hall_volt(r, volts);
    //       for (int i = 0; i < 8; i++){
    //         Serial.printf("%i volt = %f", c, volts[i]);
    //       }
    //       count++;
    //     }
    //   }
    // }
    int fromRow = -1;
    int fromCol = -1;
    int toRow = -1;
    int toCol = -1;

    int changedCount = 0;

    for (int r = 0; r < 8; r++) {
      for (int c = 0; c < 8; c++) {

        PieceType pastPiece = GameState.past_state[r][c].piecetype;
        PieceType curPiece  = GameState.cur_state[r][c].piecetype;

        if (curPiece != pastPiece) {
          same = false;
          changedCount++;

          Serial.printf("Change detected at row %i col %i: past=%i cur=%i\r\n",
                        r, c, pastPiece, curPiece);

          // Piece was removed from this square
          if (pastPiece != EMPTY && curPiece == EMPTY) {
            fromRow = r;
            fromCol = c;
          }

          // Piece was placed on this square
          else if (pastPiece == EMPTY && curPiece != EMPTY) {
            toRow = r;
            toCol = c;
          }

          // Capture / replacement case
          else if (pastPiece != EMPTY && curPiece != EMPTY && pastPiece != curPiece) {
            toRow = r;
            toCol = c;
        }
      }
    }
  }
    // wait for sure for update then send move over to website
    if (!same){
      // make sure a move has been made count to 
      if (valid_game_update()){
        // keep waiting and updating the state until you reach a valid game state that's different from the start
        Serial.printf("Game state update valid, move has been made! Piece was moved from row: %i col: %i to row: %i col: %i\r\n", fromRow, fromCol, toRow, toCol);
        commit_state();
        
      }
      else{
        Serial.println("Board changed, but move is not complete yet.");
      }
      // send the new state over wifi to web server
      // wifi_send_state(GameState);ß
      // wait for response

    }
  }
}
