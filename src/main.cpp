#include <Arduino.h>
#include <stdint.h>
#include "hall_sensor.h"
#include "leds.h"
#include "state.h"
#include "chess_wifi.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  Serial.println("ESP32 booted successfully");
  hall_init();
  leds_init();
  setup_state();

  // Save the initial board position as the baseline
  commit_state();
  // // // wifi setup
  bool connected = wifi_connect();

  if (!connected)
  {
    Serial.println("Stopping network setup because WiFi failed.");
    return;
  }

  bool serverOk = test_server_http();

  if (!serverOk)
  {
    Serial.println("HTTP test failed. Not starting WebSocket yet.");
    return;
  }

  websocket_begin();
  // print_board_state();
  //  test to see what networks are around
  //  Serial.println("Scanning WiFi networks...");
  //  int n = WiFi.scanNetworks();

  // Serial.printf("Found %i networks\n", n);

  // for (int i = 0; i < n; i++) {
  //   Serial.print(i);
  //   Serial.print(": ");
  //   Serial.print(WiFi.SSID(i));
  //   Serial.print(" RSSI=");
  //   Serial.println(WiFi.RSSI(i));
  // }
}

String expectedVirtualMove = "";

bool physicalplayer_turn = true;

int expectedFromRow = -1;
int expectedFromCol = -1;
int expectedToRow = -1;
int expectedToCol = -1;

void loop()
{
  // put your main code here, to run repeatedly:
  // Serial.println("In loop");
  // test_hall();
  // // set_led(4);
  // test_led();
  // delay(100);
  websocket_loop();

  // set a tolerance to detect change in state
  // float tol = 0.5;
  // interrupt that sets every __ ms
  // int count = 0;
  if (ready_for_state_update())
  {
    update_state();

    if (!board_scan_complete())
    {
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
    if (physicalplayer_turn)
    {
      int fromRow = -1;
      int fromCol = -1;
      int toRow = -1;
      int toCol = -1;

      int changedCount = 0;

      for (int r = 0; r < 8; r++)
      {
        for (int c = 0; c < 8; c++)
        {

          PieceType pastPiece = GameState.past_state[r][c].piecetype;
          PieceType curPiece = GameState.cur_state[r][c].piecetype;

          if (curPiece != pastPiece)
          {
            same = false;
            changedCount++;

            Serial.printf("Change detected at row %i col %i: past=%i cur=%i\r\n",
                          r, c, pastPiece, curPiece);

            // Piece was removed from this square
            if (pastPiece != EMPTY && curPiece == EMPTY)
            {
              fromRow = r;
              fromCol = c;
            }

            // Piece was placed on this square
            else if (pastPiece == EMPTY && curPiece != EMPTY)
            {
              toRow = r;
              toCol = c;
            }

            // Capture / replacement case
            else if (pastPiece != EMPTY && curPiece != EMPTY && pastPiece != curPiece)
            {
              toRow = r;
              toCol = c;
            }
          }
        }
      }
      // wait for sure for update then send move over to website
      if (!same)
      {
        // make sure a move has been made count to
        if (valid_game_update())
        {
          // keep waiting and updating the state until you reach a valid game state that's different from the start
          Serial.printf("Game state update valid, move has been made! Piece was moved from row: %i col: %i to row: %i col: %i\r\n", fromRow, fromCol, toRow, toCol);
          // send the move to the server
          send_board_move(fromRow, fromCol, toRow, toCol);
          // commit the state
          commit_state();
          // wait for server to send virtual move
          while (get_virtual_move() == "")
          {
            // keep trying to receive
            websocket_loop();
            delay(10);
          }
          String vmove = get_virtual_move();
          // get move and show it on LEDS
          show_move(vmove);
          Serial.printf("got a virtual move");
          Serial.println(vmove);
          // save virtual move
          expectedVirtualMove = vmove;
          if (!parse_uci_move(
                  expectedVirtualMove.c_str(),
                  expectedFromRow,
                  expectedFromCol,
                  expectedToRow,
                  expectedToCol))
          {
            Serial.println("Could not parse expected virtual move");
          }
          // // clear virtual move for next time
          clear_virtual_move();

          // wait for physical player to no
          physicalplayer_turn = false;
        }
        else
        {
          Serial.println("Board changed, but move is not complete yet.");
        }
        // send the new state over wifi to web server
        // wifi_send_state(GameState);ß
        // wait for response
      }
    }
    // waiting for physical player to now move opposing piece to right place to turn leds off
    else
    {
      int fromRow = -1;
      int fromCol = -1;
      int toRow = -1;
      int toCol = -1;

      int changedCount = 0;
      bool same = true;

      for (int r = 0; r < 8; r++)
      {
        for (int c = 0; c < 8; c++)
        {

          PieceType pastPiece = GameState.past_state[r][c].piecetype;
          PieceType curPiece = GameState.cur_state[r][c].piecetype;

          if (curPiece != pastPiece)
          {
            same = false;
            changedCount++;

            Serial.printf(
                "Virtual response change detected at row %i col %i: past=%i cur=%i\r\n",
                r, c, pastPiece, curPiece);

            // Piece was removed from this square
            if (pastPiece != EMPTY && curPiece == EMPTY)
            {
              fromRow = r;
              fromCol = c;
            }

            // Piece was placed on this square
            else if (pastPiece == EMPTY && curPiece != EMPTY)
            {
              toRow = r;
              toCol = c;
            }

            // Capture / replacement case
            else if (pastPiece != EMPTY && curPiece != EMPTY && pastPiece != curPiece)
            {
              toRow = r;
              toCol = c;
            }
          }
        }
      }

      if (!same)
      {
        if (valid_game_update())
        {
          Serial.printf(
              "Human moved virtual piece from row %i col %i to row %i col %i\r\n",
              fromRow, fromCol, toRow, toCol);

          bool moveMatches =
              fromRow == expectedFromRow &&
              fromCol == expectedFromCol &&
              toRow == expectedToRow &&
              toCol == expectedToCol;

          if (moveMatches)
          {
            Serial.println("Correct virtual move completed physically.");

            commit_state();

            clear_leds();

            expectedVirtualMove = "";
            expectedFromRow = -1;
            expectedFromCol = -1;
            expectedToRow = -1;
            expectedToCol = -1;

            physicalplayer_turn = true;
          }
          else
          {
            Serial.println("Wrong move made for virtual response. Keep LEDs on.");

            Serial.printf(
                "Expected from row %i col %i to row %i col %i\r\n",
                expectedFromRow,
                expectedFromCol,
                expectedToRow,
                expectedToCol);

            Serial.printf(
                "Actual from row %i col %i to row %i col %i\r\n",
                fromRow,
                fromCol,
                toRow,
                toCol);

            // Keep showing the correct move
            show_move(expectedVirtualMove);
          }
        }
        else
        {
          Serial.println("Virtual move is not complete yet. Keep LEDs on.");
        }
      }
    }
  }
}
