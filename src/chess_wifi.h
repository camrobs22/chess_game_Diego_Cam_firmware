// #include <Arduino.h>
// #include <stdint.h>
// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <WebSocketsClient.h>
// #include <ArduinoJson.h>



// bool wifi_connect();
// bool test_server_http();

// void websocket_begin();
// void websocket_loop();
// bool websocket_is_connected();

// void send_hello_to_server();

// void send_board_move(int from_row, int from_col, int to_row, int to_col);

// String get_virtual_move();

// void clear_virtual_move();

// bool parse_uci_move(const char* move, int& fromRow, int& fromCol, int& toRow, int& toCol);

// void clear_move_rejected();

// bool was_move_rejected();

#ifndef CHESS_WIFI_H
#define CHESS_WIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// WiFi / server setup
bool wifi_connect();
bool test_server_http();

// WebSocket control
void websocket_begin();
void websocket_loop();
bool websocket_is_connected();

// Optional manual hello
void send_hello_to_server();

// Send physical board move to server
void send_board_move(int from_row, int from_col, int to_row, int to_col);

// Move parsing helper
bool parse_uci_move(
  const char* move,
  int& fromRow,
  int& fromCol,
  int& toRow,
  int& toCol
);

// Virtual / remote move state
String get_virtual_move();
bool get_virtual_move_was_capture();
void clear_virtual_move();

// Invalid move state
void clear_move_rejected();
bool was_move_rejected();

// Game result states
bool was_game_won();
void clear_game_won();

bool was_game_lost();
void clear_game_lost();

bool was_game_draw();
void clear_game_draw();

// Last status/debug messages
String get_last_status_message();
void clear_last_status_message();

String get_last_rejected_move();

String get_last_capture_move();
void clear_last_capture_move();

#endif