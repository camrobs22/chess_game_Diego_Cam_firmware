// #include <Arduino.h>
// #include <stdint.h>
// #include "hall_sensor.h"
// #include "leds.h"
// #include "state.h"
// #include "chess_wifi.h"

// void setup()
// {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//   delay(2000);
//   Serial.println("ESP32 booted successfully");
//   hall_init();
//   leds_init();
//   setup_state();
//   // // // // wifi setup
//   bool connected = wifi_connect();

//   if (!connected)
//   {
//     Serial.println("Stopping network setup because WiFi failed.");
//     return;
//   }

//   bool serverOk = test_server_http();

//   if (!serverOk)
//   {
//     Serial.println("HTTP test failed. Not starting WebSocket yet.");
//     return;
//   }

//   websocket_begin();
//   // print_board_state();
//   //  test to see what networks are around
//   //  Serial.println("Scanning WiFi networks...");
//   //  int n = WiFi.scanNetworks();

//   // Serial.printf("Found %i networks\n", n);

//   // for (int i = 0; i < n; i++) {
//   //   Serial.print(i);
//   //   Serial.print(": ");
//   //   Serial.print(WiFi.SSID(i));
//   //   Serial.print(" RSSI=");
//   //   Serial.println(WiFi.RSSI(i));
//   // }
// }

// String expectedVirtualMove = "";

// bool physicalplayer_turn = true;

// int expectedFromRow = -1;
// int expectedFromCol = -1;
// int expectedToRow = -1;
// int expectedToCol = -1;

// void loop()
// { 
//   // put your main code here, to run repeatedly:
//   //test_led();
//   //test_hall();
//   // delay(100);
//   // // Serial.println("In loop");
//   // // test_hall();
//   // // // set_led(4);
//   // // test_led();
//   // // delay(100);
//   websocket_loop();

//   // set a tolerance to detect change in state
//   // float tol = 0.5;
//   // interrupt that sets every __ ms
//   // int count = 0;
//   if (ready_for_state_update())
//   {
//     update_state();

//     if (!board_scan_complete())
//     {
//       return;
//     }
//     // check to see if anything has changed
//     bool same = true;
//     // rememebr pos of change
//     // int col_change;
//     // int row_change;

//     // int col_change2;
//     // int row_change2;

//     // // int start_col;
//     // // int start_row;
//     // // int end_col;
//     // // int end_row;

//     // for (int r = 0; r < 8; r++) {
//     //   for (int c = 0; c < 8; c++) {
//     //     // if (GameState.cur_state[r][c] > GameState.past_state[r][c] + tol || GameState.cur_state[r][c] < GameState.past_state[r][c] - tol) {
//     //     // might need to change to adc volt values if this doesn't work and have a statement like above
//     //     // need to change this its not detecting a change
//     //     if (GameState.cur_state[r][c].piecetype != GameState.past_state[r][c].piecetype){
//     //       same = false;
//     //       // means we have already detected a change
//     //       if (count > 0){
//     //         if (c != col_change || r != row_change){
//     //           col_change2 = c;
//     //           row_change2 = r;
//     //           count = 0;
//     //         }
//     //       }
//     //       else{
//     //         col_change = c;
//     //         row_change = r;
//     //       }

//     //       Serial.printf("inside false statement row of change and col of change %i %i\r\n", r, c);
//     //       float volts[8];
//     //       get_hall_volt(r, volts);
//     //       for (int i = 0; i < 8; i++){
//     //         Serial.printf("%i volt = %f", c, volts[i]);
//     //       }
//     //       count++;
//     //     }
//     //   }
//     // }
//     if (physicalplayer_turn)
//     {
//       int fromRow = -1;
//       int fromCol = -1;
//       int toRow = -1;
//       int toCol = -1;

//       int changedCount = 0;

//       for (int r = 0; r < 8; r++)
//       {
//         for (int c = 0; c < 8; c++)
//         {

//           PieceType pastPiece = GameState.past_state[r][c].piecetype;
//           PieceType curPiece = GameState.cur_state[r][c].piecetype;

//           if (curPiece != pastPiece)
//           {
//             same = false;
//             changedCount++;

//             Serial.printf("Change detected at row %i col %i: past=%i cur=%i\r\n",
//                           r, c, pastPiece, curPiece);

//             // Piece was removed from this square
//             if (pastPiece != EMPTY && curPiece == EMPTY)
//             {
//               fromRow = r;
//               fromCol = c;
//             }

//             // Piece was placed on this square
//             else if (pastPiece == EMPTY && curPiece != EMPTY)
//             {
//               toRow = r;
//               toCol = c;
//             }

//             // Capture / replacement case
//             else if (pastPiece != EMPTY && curPiece != EMPTY && pastPiece != curPiece)
//             {
//               toRow = r;
//               toCol = c;
//             }
//           }
//         }
//       }
//       // wait for sure for update then send move over to website
//       if (!same)
//       {
//         // make sure a move has been made count to
//         if (valid_game_update())
//         {
//           // keep waiting and updating the state until you reach a valid game state that's different from the start
//           Serial.printf("Game state update valid, move has been made! Piece was moved from row: %i col: %i to row: %i col: %i\r\n", fromRow, fromCol, toRow, toCol);
//           // clear error move:
//           clear_move_rejected();
//           // send the move to the server
//           send_board_move(fromRow, fromCol, toRow, toCol);
//           // wait to see if it is a validmove
//           unsigned long waitStart = millis();
//           bool rejected = false;

//           while (millis() - waitStart < 1500) {
//             websocket_loop();

//             if (was_move_rejected()) {
//               rejected = true;
//               clear_move_rejected();
//               set_invalid_move_leds();
//               break;
//             }

//             delay(10);
//           }

//           if (rejected) {
//             Serial.println("Not a valid move. Move the piece back and redo your turn.");
    

//             // Do NOT commit the invalid board state
//             return;
//           }
//           // clear_leds();
//           // commit the state
//           commit_state();
//           // wait for server to send virtual move
//           while (get_virtual_move() == "")
//           {
//             // keep trying to receive
//             websocket_loop();
//             delay(10);
//           }
//           String vmove = get_virtual_move();
//           // get move and show it on LEDS
//           show_move(vmove);
//           Serial.printf("got a virtual move");
//           Serial.println(vmove);
//           // save virtual move
//           expectedVirtualMove = vmove;
//           if (!parse_uci_move(
//                   expectedVirtualMove.c_str(),
//                   expectedFromRow,
//                   expectedFromCol,
//                   expectedToRow,
//                   expectedToCol))
//           {
//             Serial.println("Could not parse expected virtual move");
//           }
//           // // clear virtual move for next time
//           clear_virtual_move();

//           // wait for physical player to now
//           physicalplayer_turn = false;
//         }
//         else
//         {
//           Serial.printf("Board changed, but move is not complete yet. From row/col = %i %i To row/col = %i %i\r\n",fromRow, fromCol, toRow, toCol);
//           clear_leds();
//         }
//         // send the new state over wifi to web server
//         // wifi_send_state(GameState);ß
//         // wait for response
//       }
//     }
//     // waiting for physical player to now move opposing piece to right place to turn leds off
//     else
//     {
//       int fromRow = -1;
//       int fromCol = -1;
//       int toRow = -1;
//       int toCol = -1;

//       int changedCount = 0;
//       bool same = true;

//       for (int r = 0; r < 8; r++)
//       {
//         for (int c = 0; c < 8; c++)
//         {

//           PieceType pastPiece = GameState.past_state[r][c].piecetype;
//           PieceType curPiece = GameState.cur_state[r][c].piecetype;

//           if (curPiece != pastPiece)
//           {
//             same = false;
//             changedCount++;

//             Serial.printf(
//                 "Virtual response change detected at row %i col %i: past=%i cur=%i\r\n",
//                 r, c, pastPiece, curPiece);

//             // Piece was removed from this square
//             if (pastPiece != EMPTY && curPiece == EMPTY)
//             {
//               fromRow = r;
//               fromCol = c;
//             }

//             // Piece was placed on this square
//             else if (pastPiece == EMPTY && curPiece != EMPTY)
//             {
//               toRow = r;
//               toCol = c;
//             }

//             // Capture / replacement case
//             else if (pastPiece != EMPTY && curPiece != EMPTY && pastPiece != curPiece)
//             {
//               toRow = r;
//               toCol = c;
//             }
//           }
//         }
//       }

//       if (!same)
//       {
//         if (valid_game_update())
//         {
//           Serial.printf(
//               "Human moved virtual piece from row %i col %i to row %i col %i\r\n",
//               fromRow, fromCol, toRow, toCol);

//           bool moveMatches =
//               fromRow == expectedFromRow &&
//               fromCol == expectedFromCol &&
//               toRow == expectedToRow &&
//               toCol == expectedToCol;

//           if (moveMatches)
//           {
//             Serial.println("Correct virtual move completed physically.");

//             commit_state();

//             clear_leds();

//             expectedVirtualMove = "";
//             expectedFromRow = -1;
//             expectedFromCol = -1;
//             expectedToRow = -1;
//             expectedToCol = -1;

//             physicalplayer_turn = true;
//           }
//           else
//           {
//             Serial.println("Wrong move made for virtual response. Keep LEDs on.");

//             Serial.printf(
//                 "Expected from row %i col %i to row %i col %i\r\n",
//                 expectedFromRow,
//                 expectedFromCol,
//                 expectedToRow,
//                 expectedToCol);

//             Serial.printf(
//                 "Actual from row %i col %i to row %i col %i\r\n",
//                 fromRow,
//                 fromCol,
//                 toRow,
//                 toCol);

//             // Keep showing the correct move
//             show_move(expectedVirtualMove);
//           }
//         }
//         else
//         {
//           Serial.println("Virtual move is not complete yet. Keep LEDs on.");
//         }
//       }
//     }
//   }
// }

// #include <Arduino.h>
// #include <stdint.h>
// #include "hall_sensor.h"
// #include "leds.h"
// #include "state.h"
// #include "chess_wifi.h"

// #define ENABLE_NETWORK_GAME 1

// bool networkGameStarted = false;

// String expectedVirtualMove = "";

// bool physicalplayer_turn = true;
// bool invalidMoveMode = false;

// int expectedFromRow = -1;
// int expectedFromCol = -1;
// int expectedToRow = -1;
// int expectedToCol = -1;

// bool board_matches_past_state() {
//   for (int r = 0; r < 8; r++) {
//     for (int c = 0; c < 8; c++) {
//       if (GameState.cur_state[r][c].piecetype != GameState.past_state[r][c].piecetype) {
//         return false;
//       }
//     }
//   }

//   return true;
// }

// void reset_expected_virtual_move() {
//   expectedVirtualMove = "";
//   expectedFromRow = -1;
//   expectedFromCol = -1;
//   expectedToRow = -1;
//   expectedToCol = -1;
// }

// void setup()
// {
//   Serial.begin(115200);
//   delay(2000);

//   Serial.println("ESP32 booted successfully");

//   hall_init();
//   leds_init();
//   setup_state();

// #if ENABLE_NETWORK_GAME
//   bool connected = wifi_connect();

//   if (!connected)
//   {
//     Serial.println("WiFi failed. Continuing without server/LED game events.");
//   }
//   else
//   {
//     bool serverOk = test_server_http();

//     if (!serverOk)
//     {
//       Serial.println("HTTP test failed. Starting WebSocket anyway so reconnect can keep trying.");
//     }

//     websocket_begin();
//     networkGameStarted = true;
//   }
// #endif
// }

// void loop()
// {
// #if ENABLE_NETWORK_GAME
//   if (networkGameStarted) {
//     websocket_loop();
//   }
// #endif

//   if (!ready_for_state_update())
//   {
//     return;
//   }

//   update_state();

//   if (!board_scan_complete())
//   {
//     return;
//   }

//   bool same = true;

//   if (physicalplayer_turn)
//   {
//     int fromRow = -1;
//     int fromCol = -1;
//     int toRow = -1;
//     int toCol = -1;

//     int changedCount = 0;

//     for (int r = 0; r < 8; r++)
//     {
//       for (int c = 0; c < 8; c++)
//       {
//         PieceType pastPiece = GameState.past_state[r][c].piecetype;
//         PieceType curPiece = GameState.cur_state[r][c].piecetype;

//         if (curPiece != pastPiece)
//         {
//           same = false;
//           changedCount++;

//           Serial.printf(
//             "Change detected at row %i col %i: past=%i cur=%i\r\n",
//             r,
//             c,
//             pastPiece,
//             curPiece
//           );

//           if (pastPiece != EMPTY && curPiece == EMPTY)
//           {
//             fromRow = r;
//             fromCol = c;
//           }
//           else if (pastPiece == EMPTY && curPiece != EMPTY)
//           {
//             toRow = r;
//             toCol = c;
//           }
//           else if (pastPiece != EMPTY && curPiece != EMPTY && pastPiece != curPiece)
//           {
//             toRow = r;
//             toCol = c;
//           }
//         }
//       }
//     }

//     if (same)
//     {
//       if (invalidMoveMode)
//       {
//         clear_leds();
//         invalidMoveMode = false;
//         Serial.println("Invalid move was undone. LEDs cleared.");
//       }

//       return;
//     }

//     if (valid_game_update())
//     {
//       Serial.printf(
//         "Game state update valid, move has been made! Piece was moved from row: %i col: %i to row: %i col: %i\r\n",
//         fromRow,
//         fromCol,
//         toRow,
//         toCol
//       );

//       clear_move_rejected();
//       clear_virtual_move();

//       send_board_move(fromRow, fromCol, toRow, toCol);

//       unsigned long waitStart = millis();
//       bool rejected = false;

//       while (millis() - waitStart < 1500)
//       {
// #if ENABLE_NETWORK_GAME
//         if (networkGameStarted) {
//           websocket_loop();
//         }
// #endif

//         if (was_move_rejected())
//         {
//           rejected = true;
//           clear_move_rejected();
//           break;
//         }

//         delay(10);
//       }

//       if (rejected)
//       {
//         Serial.println("Not a valid move. Move the piece back and redo your turn.");

//         show_invalid_move();
//         invalidMoveMode = true;

//         // Do NOT commit the invalid board state.
//         return;
//       }

//       if (invalidMoveMode)
//       {
//         clear_leds();
//         invalidMoveMode = false;
//       }

//       commit_state();

//       Serial.println("Physical move accepted locally. Waiting for virtual/server move.");

//       unsigned long virtualWaitStart = millis();

//       while (get_virtual_move() == "")
//       {
// #if ENABLE_NETWORK_GAME
//         if (networkGameStarted) {
//           websocket_loop();
//         }
// #endif

//         if (millis() - virtualWaitStart > 10000)
//         {
//           Serial.println("Timed out waiting for virtual move.");
//           virtualWaitStart = millis();
//           return;
//         }

//         delay(10);
//       }

//       String vmove = get_virtual_move();

//       Serial.print("Got virtual move: ");
//       Serial.println(vmove);

//       if (get_virtual_move_was_capture())
//       {
//         show_capture_move(vmove);
//       }
//       else
//       {
//         show_move(vmove);
//       }

//       expectedVirtualMove = vmove;

//       if (!parse_uci_move(
//             expectedVirtualMove.c_str(),
//             expectedFromRow,
//             expectedFromCol,
//             expectedToRow,
//             expectedToCol))
//       {
//         Serial.println("Could not parse expected virtual move.");
//         reset_expected_virtual_move();
//         clear_virtual_move();
//         return;
//       }

//       clear_virtual_move();

//       physicalplayer_turn = false;
//     }
//     else
//     {
//       Serial.printf(
//         "Board changed, but move is not complete yet. From row/col = %i %i To row/col = %i %i\r\n",
//         fromRow,
//         fromCol,
//         toRow,
//         toCol
//       );
//     }
//   }

//   else
//   {
//     int fromRow = -1;
//     int fromCol = -1;
//     int toRow = -1;
//     int toCol = -1;

//     int changedCount = 0;
//     bool same = true;

//     for (int r = 0; r < 8; r++)
//     {
//       for (int c = 0; c < 8; c++)
//       {
//         PieceType pastPiece = GameState.past_state[r][c].piecetype;
//         PieceType curPiece = GameState.cur_state[r][c].piecetype;

//         if (curPiece != pastPiece)
//         {
//           same = false;
//           changedCount++;

//           Serial.printf(
//             "Virtual response change detected at row %i col %i: past=%i cur=%i\r\n",
//             r,
//             c,
//             pastPiece,
//             curPiece
//           );

//           if (pastPiece != EMPTY && curPiece == EMPTY)
//           {
//             fromRow = r;
//             fromCol = c;
//           }
//           else if (pastPiece == EMPTY && curPiece != EMPTY)
//           {
//             toRow = r;
//             toCol = c;
//           }
//           else if (pastPiece != EMPTY && curPiece != EMPTY && pastPiece != curPiece)
//           {
//             toRow = r;
//             toCol = c;
//           }
//         }
//       }
//     }

//     if (same)
//     {
//       return;
//     }

//     if (board_matches_past_state())
//     {
//       Serial.println("Board returned to previous state. Keep showing expected virtual move.");
//       show_move(expectedVirtualMove);
//       return;
//     }

//     if (valid_game_update())
//     {
//       Serial.printf(
//         "Human moved virtual piece from row %i col %i to row %i col %i\r\n",
//         fromRow,
//         fromCol,
//         toRow,
//         toCol
//       );

//       bool moveMatches =
//         fromRow == expectedFromRow &&
//         fromCol == expectedFromCol &&
//         toRow == expectedToRow &&
//         toCol == expectedToCol;

//       if (moveMatches)
//       {
//         Serial.println("Correct virtual move completed physically.");

//         commit_state();

//         clear_leds();

//         reset_expected_virtual_move();

//         physicalplayer_turn = true;
//       }
//       else
//       {
//         Serial.println("Wrong move made for virtual response. Put it back or complete the highlighted move.");

//         Serial.printf(
//           "Expected from row %i col %i to row %i col %i\r\n",
//           expectedFromRow,
//           expectedFromCol,
//           expectedToRow,
//           expectedToCol
//         );

//         Serial.printf(
//           "Actual from row %i col %i to row %i col %i\r\n",
//           fromRow,
//           fromCol,
//           toRow,
//           toCol
//         );

//         show_move(expectedVirtualMove);
//         return;
//       }
//     }
//     else
//     {
//       Serial.println("Virtual move is not complete yet. Keep LEDs on.");
//       show_move(expectedVirtualMove);
//       return;
//     }
//   }
// }

#include <Arduino.h>
#include <stdint.h>
#include "hall_sensor.h"
#include "leds.h"
#include "state.h"
#include "chess_wifi.h"

#define ENABLE_NETWORK_GAME 1

bool networkGameStarted = false;

String expectedVirtualMove = "";

bool physicalplayer_turn = true;
bool invalidMoveMode = false;
bool gameOverMode = false;
String pendingPhysicalCastlingMove = "";

int expectedFromRow = -1;
int expectedFromCol = -1;
int expectedToRow = -1;
int expectedToCol = -1;

struct CastlingLayout {
  bool valid;
  int row;
  int kingFromCol;
  int kingToCol;
  int rookFromCol;
  int rookToCol;
};

CastlingLayout get_castling_layout(const String& move) {
  CastlingLayout layout = {false, -1, -1, -1, -1, -1};

  if (move != "e1g1" && move != "e1c1" &&
      move != "e8g8" && move != "e8c8") {
    return layout;
  }

  bool kingSide = move.charAt(2) == 'g';
  layout.valid = true;
  layout.row = move.charAt(1) == '8' ? 0 : 7;
  layout.kingFromCol = 4;
  layout.kingToCol = kingSide ? 6 : 2;
  layout.rookFromCol = kingSide ? 7 : 0;
  layout.rookToCol = kingSide ? 5 : 3;
  return layout;
}

bool board_matches_castling_move(const String& move) {
  CastlingLayout layout = get_castling_layout(move);
  if (!layout.valid) {
    return false;
  }

  PieceType pieceColor =
    GameState.past_state[layout.row][layout.kingFromCol].piecetype;

  if (pieceColor == EMPTY ||
      GameState.past_state[layout.row][layout.rookFromCol].piecetype != pieceColor ||
      GameState.past_state[layout.row][layout.kingToCol].piecetype != EMPTY ||
      GameState.past_state[layout.row][layout.rookToCol].piecetype != EMPTY) {
    return false;
  }

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      PieceType expected = GameState.past_state[row][col].piecetype;

      if (row == layout.row) {
        if (col == layout.kingFromCol || col == layout.rookFromCol) {
          expected = EMPTY;
        }
        else if (col == layout.kingToCol || col == layout.rookToCol) {
          expected = pieceColor;
        }
      }

      if (GameState.cur_state[row][col].piecetype != expected) {
        return false;
      }
    }
  }

  return true;
}

void service_network() {
#if ENABLE_NETWORK_GAME
  if (networkGameStarted) {
    websocket_loop();
  }
#endif
}

bool board_matches_past_state() {
  for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
      if (GameState.cur_state[r][c].piecetype != GameState.past_state[r][c].piecetype) {
        return false;
      }
    }
  }

  return true;
}

void reset_expected_virtual_move() {
  expectedVirtualMove = "";
  expectedFromRow = -1;
  expectedFromCol = -1;
  expectedToRow = -1;
  expectedToCol = -1;
}

bool finish_game_if_ready() {
  if (gameOverMode ||
      (!was_game_won() && !was_game_lost() && !was_game_draw()) ||
      game_result_waiting_for_virtual_move()) {
    return false;
  }

  gameOverMode = true;

  String statusMessage = get_last_status_message();
  if (statusMessage != "") {
    Serial.print("Game over: ");
    Serial.println(statusMessage);
  }

  if (was_game_won()) {
    show_win_animation();
  }
  else if (was_game_lost()) {
    show_loss_animation();
  }
  else {
    show_draw_animation();
  }

  return true;
}

bool activate_queued_virtual_move() {
  String vmove = get_virtual_move();
  if (vmove == "") {
    return false;
  }

  physicalplayer_turn = false;

  Serial.print("Got virtual move: ");
  Serial.println(vmove);

  if (get_castling_layout(vmove).valid) {
    show_castling_move(vmove);
  }
  else if (get_virtual_move_was_capture()) {
    show_capture_move(vmove);
  }
  else {
    show_move(vmove);
  }

  expectedVirtualMove = vmove;

  if (!parse_uci_move(
        expectedVirtualMove.c_str(),
        expectedFromRow,
        expectedFromCol,
        expectedToRow,
        expectedToCol)) {
    Serial.println("Could not parse expected virtual move.");
    reset_expected_virtual_move();
    clear_virtual_move();
    physicalplayer_turn = true;
    return false;
  }

  clear_virtual_move();
  return true;
}

void wait_for_virtual_move_after_physical_move() {
  physicalplayer_turn = false;

  Serial.println("Physical move accepted locally. Waiting for virtual/server move.");

  unsigned long virtualWaitStart = millis();

  while (get_virtual_move() == "") {
    service_network();

    if ((was_game_won() || was_game_lost() || was_game_draw()) &&
        !game_result_waiting_for_virtual_move()) {
      Serial.println("Game ended. No virtual response move needed.");
      clear_virtual_move();
      reset_expected_virtual_move();
      finish_game_if_ready();
      return;
    }

    if (millis() - virtualWaitStart > 10000) {
      Serial.println("Still waiting for virtual move. Do not move pieces yet.");
      virtualWaitStart = millis();
    }

    delay(10);
  }

  activate_queued_virtual_move();
}

void setup_hardware_test()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("ESP32 booted successfully");

  hall_init();
  leds_init();

  Serial.println("Starting full-board LED test");
  test_led();
  Serial.println("Starting continuous full-board hall sensor test");

// #if ENABLE_NETWORK_GAME
//   bool connected = wifi_connect();

//   if (!connected)
//   {
//     Serial.println("WiFi failed. Continuing without server/LED game events.");
//   }
//   else
//   {
//     bool serverOk = test_server_http();

//     if (!serverOk)
//     {
//       Serial.println("HTTP test failed. Starting WebSocket anyway so reconnect can keep trying.");
//     }

//     websocket_begin();
//     networkGameStarted = true;
//   }
// #endif
}

void loop_hardware_test()
{
  test_hall();
  delay(500);
  // service_network();

  // if (gameOverMode) {
  //   return;
  // }

  // if (was_game_won() || was_game_lost() || was_game_draw()) {
  //   Serial.println("Game over flag received. Stopping board move detection.");
  //   gameOverMode = true;
  //   return;
  // }

  // if (!ready_for_state_update())
  // {
  //   return;
  // }

  // update_state();

  // if (!board_scan_complete())
  // {
  //   return;
  // }

  // bool same = true;

  // if (physicalplayer_turn)
  // {
  //   int fromRow = -1;
  //   int fromCol = -1;
  //   int toRow = -1;
  //   int toCol = -1;

  //   int changedCount = 0;

  //   for (int r = 0; r < 8; r++)
  //   {
  //     for (int c = 0; c < 8; c++)
  //     {
  //       PieceType pastPiece = GameState.past_state[r][c].piecetype;
  //       PieceType curPiece = GameState.cur_state[r][c].piecetype;

  //       if (curPiece != pastPiece)
  //       {
  //         same = false;
  //         changedCount++;

  //         Serial.printf(
  //           "Change detected at row %i col %i: past=%i cur=%i\r\n",
  //           r,
  //           c,
  //           pastPiece,
  //           curPiece
  //         );

  //         if (pastPiece != EMPTY && curPiece == EMPTY)
  //         {
  //           fromRow = r;
  //           fromCol = c;
  //         }
  //         else if (pastPiece == EMPTY && curPiece != EMPTY)
  //         {
  //           toRow = r;
  //           toCol = c;
  //         }
  //         else if (pastPiece != EMPTY && curPiece != EMPTY && pastPiece != curPiece)
  //         {
  //           toRow = r;
  //           toCol = c;
  //         }
  //       }
  //     }
  //   }

  //   if (same)
  //   {
  //     if (invalidMoveMode)
  //     {
  //       clear_leds();
  //       invalidMoveMode = false;
  //       Serial.println("Invalid move was undone. LEDs cleared.");
  //     }

  //     return;
  //   }

  //   if (valid_game_update())
  //   {
  //     Serial.printf(
  //       "Game state update valid, move has been made! Piece was moved from row: %i col: %i to row: %i col: %i\r\n",
  //       fromRow,
  //       fromCol,
  //       toRow,
  //       toCol
  //     );

  //     clear_move_rejected();
  //     clear_virtual_move();

  //     send_board_move(fromRow, fromCol, toRow, toCol);

  //     unsigned long waitStart = millis();
  //     bool rejected = false;

  //     while (millis() - waitStart < 1500)
  //     {
  //       service_network();

  //       if (was_move_rejected())
  //       {
  //         rejected = true;
  //         clear_move_rejected();
  //         break;
  //       }

  //       if (was_game_won() || was_game_lost() || was_game_draw()) {
  //         break;
  //       }

  //       delay(10);
  //     }

  //     if (rejected)
  //     {
  //       Serial.println("Not a valid move. Move the piece back and redo your turn.");

  //       show_invalid_move();
  //       invalidMoveMode = true;

  //       // Do NOT commit the invalid board state.
  //       return;
  //     }

  //     if (invalidMoveMode)
  //     {
  //       clear_leds();
  //       invalidMoveMode = false;
  //     }

  //     commit_state();

  //     if (was_game_won() || was_game_lost() || was_game_draw()) {
  //       Serial.println("Move ended the game. Not waiting for virtual response.");
  //       gameOverMode = true;
  //       return;
  //     }

  //     wait_for_virtual_move_after_physical_move();
  //   }
  //   else
  //   {
  //     Serial.printf(
  //       "Board changed, but move is not complete yet. From row/col = %i %i To row/col = %i %i\r\n",
  //       fromRow,
  //       fromCol,
  //       toRow,
  //       toCol
  //     );
  //   }
  // }

  // else
  // {
  //   int fromRow = -1;
  //   int fromCol = -1;
  //   int toRow = -1;
  //   int toCol = -1;

  //   int changedCount = 0;
  //   bool same = true;

  //   for (int r = 0; r < 8; r++)
  //   {
  //     for (int c = 0; c < 8; c++)
  //     {
  //       PieceType pastPiece = GameState.past_state[r][c].piecetype;
  //       PieceType curPiece = GameState.cur_state[r][c].piecetype;

  //       if (curPiece != pastPiece)
  //       {
  //         same = false;
  //         changedCount++;

  //         Serial.printf(
  //           "Virtual response change detected at row %i col %i: past=%i cur=%i\r\n",
  //           r,
  //           c,
  //           pastPiece,
  //           curPiece
  //         );

  //         if (pastPiece != EMPTY && curPiece == EMPTY)
  //         {
  //           fromRow = r;
  //           fromCol = c;
  //         }
  //         else if (pastPiece == EMPTY && curPiece != EMPTY)
  //         {
  //           toRow = r;
  //           toCol = c;
  //         }
  //         else if (pastPiece != EMPTY && curPiece != EMPTY && pastPiece != curPiece)
  //         {
  //           toRow = r;
  //           toCol = c;
  //         }
  //       }
  //     }
  //   }

  //   if (same)
  //   {
  //     return;
  //   }

  //   if (board_matches_past_state())
  //   {
  //     Serial.println("Board returned to previous state. Keep showing expected virtual move.");

  //     if (expectedVirtualMove != "") {
  //       show_move(expectedVirtualMove);
  //     }

  //     return;
  //   }

  //   if (valid_game_update())
  //   {
  //     Serial.printf(
  //       "Human moved virtual piece from row %i col %i to row %i col %i\r\n",
  //       fromRow,
  //       fromCol,
  //       toRow,
  //       toCol
  //     );

  //     bool moveMatches =
  //       fromRow == expectedFromRow &&
  //       fromCol == expectedFromCol &&
  //       toRow == expectedToRow &&
  //       toCol == expectedToCol;

  //     if (moveMatches)
  //     {
  //       Serial.println("Correct virtual move completed physically.");

  //       commit_state();

  //       clear_leds();

  //       reset_expected_virtual_move();

  //       physicalplayer_turn = true;
  //     }
  //     else
  //     {
  //       Serial.println("Wrong move made for virtual response. Put it back or complete the highlighted move.");

  //       Serial.printf(
  //         "Expected from row %i col %i to row %i col %i\r\n",
  //         expectedFromRow,
  //         expectedFromCol,
  //         expectedToRow,
  //         expectedToCol
  //       );

  //       Serial.printf(
  //         "Actual from row %i col %i to row %i col %i\r\n",
  //         fromRow,
  //         fromCol,
  //         toRow,
  //         toCol
  //       );

  //       if (expectedVirtualMove != "") {
  //         show_move(expectedVirtualMove);
  //       }

  //       return;
  //     }
  //   }
  //   else
  //   {
  //     Serial.println("Virtual move is not complete yet. Keep LEDs on.");

  //     if (expectedVirtualMove != "") {
  //       show_move(expectedVirtualMove);
  //     }

  //     return;
  //   }
  // }
}

struct DetectedMove {
  bool boardChanged;
  int changedSquareCount;
  int fromRow;
  int fromCol;
  int toRow;
  int toCol;
};

String castling_move_for_detection(const DetectedMove& move) {
  if (move.fromRow != move.toRow ||
      (move.fromRow != 0 && move.fromRow != 7) ||
      move.fromCol != 4 ||
      (move.toCol != 2 && move.toCol != 6)) {
    return "";
  }

  String castlingMove = "";
  castlingMove += 'e';
  castlingMove += move.fromRow == 0 ? '8' : '1';
  castlingMove += move.toCol == 6 ? 'g' : 'c';
  castlingMove += move.toRow == 0 ? '8' : '1';
  return castlingMove;
}

DetectedMove detect_board_move(const char* logPrefix) {
  DetectedMove move = {false, 0, -1, -1, -1, -1};

  const char* castlingMoves[] = {"e1g1", "e1c1", "e8g8", "e8c8"};
  for (const char* castlingMove : castlingMoves) {
    if (board_matches_castling_move(castlingMove)) {
      CastlingLayout layout = get_castling_layout(castlingMove);
      move.boardChanged = true;
      move.changedSquareCount = 4;
      move.fromRow = layout.row;
      move.fromCol = layout.kingFromCol;
      move.toRow = layout.row;
      move.toCol = layout.kingToCol;
      Serial.printf("%s: completed castling move %s\r\n", logPrefix, castlingMove);
      return move;
    }
  }

  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      PieceType pastPiece = GameState.past_state[row][col].piecetype;
      PieceType currentPiece = GameState.cur_state[row][col].piecetype;

      if (currentPiece == pastPiece) {
        continue;
      }

      move.boardChanged = true;
      move.changedSquareCount++;
      Serial.printf(
        "%s at row %i col %i: past=%i cur=%i\r\n",
        logPrefix,
        row,
        col,
        pastPiece,
        currentPiece
      );

      if (pastPiece != EMPTY && currentPiece == EMPTY) {
        move.fromRow = row;
        move.fromCol = col;
      }
      else if (pastPiece == EMPTY && currentPiece != EMPTY) {
        move.toRow = row;
        move.toCol = col;
      }
      else if (pastPiece != EMPTY &&
               currentPiece != EMPTY &&
               pastPiece != currentPiece) {
        move.toRow = row;
        move.toCol = col;
      }
    }
  }

  return move;
}

bool expected_virtual_squares_are_complete() {
  if (expectedFromRow < 0 || expectedFromCol < 0 ||
      expectedToRow < 0 || expectedToCol < 0) {
    return false;
  }

  PieceType movingPiece =
    GameState.past_state[expectedFromRow][expectedFromCol].piecetype;

  return movingPiece != EMPTY &&
         GameState.cur_state[expectedFromRow][expectedFromCol].piecetype == EMPTY &&
         GameState.cur_state[expectedToRow][expectedToCol].piecetype == movingPiece;
}

void complete_expected_virtual_move() {
  Serial.println("Correct virtual move completed physically.");

  commit_state();
  clear_leds();
  reset_expected_virtual_move();
  physicalplayer_turn = true;

  if (was_game_won() || was_game_lost() || was_game_draw()) {
    clear_game_result_virtual_wait();
    finish_game_if_ready();
  }
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("ESP32 booted successfully");

  hall_init();
  leds_init();
  setup_state();

#if ENABLE_NETWORK_GAME
  bool connected = wifi_connect();

  if (!connected) {
    Serial.println("WiFi failed. Continuing without server/LED game events.");
  }
  else {
    bool serverOk = test_server_http();

    if (!serverOk) {
      Serial.println("HTTP test failed. Starting WebSocket anyway so reconnect can keep trying.");
    }

    websocket_begin();
    networkGameStarted = true;
  }
#endif
}

void loop()
{
 // loop_hardware_test();
  service_network();

  if (gameOverMode || finish_game_if_ready()) {
    return;
  }

  if (physicalplayer_turn && get_virtual_move() != "") {
    activate_queued_virtual_move();
  }

  if (!ready_for_state_update()) {
    return;
  }

  update_state();

  if (!board_scan_complete()) {
    return;
  }

  if (physicalplayer_turn) {
    DetectedMove move = detect_board_move("Change detected");

    if (invalidMoveMode) {
      if (board_matches_past_state()) {
        clear_leds();
        invalidMoveMode = false;
        Serial.println("Invalid move was undone. Board ready for another move.");
      }
      return;
    }

    if (!move.boardChanged) {
      if (pendingPhysicalCastlingMove != "") {
        clear_leds();
        pendingPhysicalCastlingMove = "";
        Serial.println("Castling attempt was undone. LEDs cleared.");
      }

      return;
    }

    String castlingMove = castling_move_for_detection(move);
    bool completedPhysicalCastling =
      castlingMove != "" && board_matches_castling_move(castlingMove);

    if (castlingMove != "" && !completedPhysicalCastling) {
      if (pendingPhysicalCastlingMove != castlingMove) {
        pendingPhysicalCastlingMove = castlingMove;
        show_castling_move(castlingMove);
      }

      Serial.println("Castling started. Move the rook to complete the move.");
      return;
    }

    if (move.fromRow < 0 || move.fromCol < 0 ||
        move.toRow < 0 || move.toCol < 0) {
      Serial.printf(
        "Board changed, but move is not complete yet. From row/col = %i %i To row/col = %i %i\r\n",
        move.fromRow,
        move.fromCol,
        move.toRow,
        move.toCol
      );
      return;
    }

    if (move.changedSquareCount != 2 && !completedPhysicalCastling) {
      Serial.printf(
        "Board has %i changed squares; waiting for a stable two-square move.\r\n",
        move.changedSquareCount
      );
      return;
    }

    if (!valid_game_update()) {
      Serial.printf(
        "Board changed, but move is not complete yet. From row/col = %i %i To row/col = %i %i\r\n",
        move.fromRow,
        move.fromCol,
        move.toRow,
        move.toCol
      );
      return;
    }

    Serial.printf(
      "Game state update valid. Piece moved from row %i col %i to row %i col %i\r\n",
      move.fromRow,
      move.fromCol,
      move.toRow,
      move.toCol
    );

    clear_move_rejected();
    clear_virtual_move();
    send_board_move(move.fromRow, move.fromCol, move.toRow, move.toCol);

    unsigned long waitStart = millis();
    bool rejected = false;

    while (millis() - waitStart < 1500) {
      service_network();

      if (was_move_rejected()) {
        rejected = true;
        clear_move_rejected();
        break;
      }

      if (was_game_won() || was_game_lost() || was_game_draw()) {
        break;
      }

      delay(10);
    }

    if (rejected) {
      Serial.println("Not a valid move. Move the piece back and redo your turn.");
      show_invalid_move();
      invalidMoveMode = true;
      return;
    }

    if (invalidMoveMode) {
      clear_leds();
      invalidMoveMode = false;
    }

    commit_state();
    pendingPhysicalCastlingMove = "";

    if (finish_game_if_ready()) {
      return;
    }

    wait_for_virtual_move_after_physical_move();
    return;
  }

  DetectedMove move = detect_board_move("Virtual response change detected");

  if (!move.boardChanged) {
    return;
  }

  if (get_castling_layout(expectedVirtualMove).valid) {
    if (!board_matches_castling_move(expectedVirtualMove)) {
      Serial.println("Castling is not complete yet. Move both the king and rook to the highlighted squares.");
      return;
    }

    complete_expected_virtual_move();
    return;
  }

  if (expected_virtual_squares_are_complete()) {
    complete_expected_virtual_move();
    return;
  }

  if (move.changedSquareCount != 2) {
    Serial.printf(
      "Virtual move has %i changed squares; waiting for a stable two-square move.\r\n",
      move.changedSquareCount
    );
    if (expectedVirtualMove != "") {
      show_move(expectedVirtualMove);
    }
    return;
  }

  if (!valid_game_update()) {
    Serial.println("Virtual move is not complete yet. Keep LEDs on.");
    if (expectedVirtualMove != "") {
      show_move(expectedVirtualMove);
    }
    return;
  }

  Serial.printf(
    "Human moved virtual piece from row %i col %i to row %i col %i\r\n",
    move.fromRow,
    move.fromCol,
    move.toRow,
    move.toCol
  );

  bool moveMatches =
    move.fromRow == expectedFromRow &&
    move.fromCol == expectedFromCol &&
    move.toRow == expectedToRow &&
    move.toCol == expectedToCol;

  if (!moveMatches) {
    Serial.println("Wrong move made for virtual response. Put it back or complete the highlighted move.");
    Serial.printf(
      "Expected from row %i col %i to row %i col %i\r\n",
      expectedFromRow,
      expectedFromCol,
      expectedToRow,
      expectedToCol
    );
    Serial.printf(
      "Actual from row %i col %i to row %i col %i\r\n",
      move.fromRow,
      move.fromCol,
      move.toRow,
      move.toCol
    );

    show_invalid_move();
    if (expectedVirtualMove != "") {
      show_move(expectedVirtualMove);
    }
    return;
  }

  complete_expected_virtual_move();
}
