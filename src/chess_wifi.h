#include <Arduino.h>
#include <stdint.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>



bool wifi_connect();
bool test_server_http();

void websocket_begin();
void websocket_loop();
bool websocket_is_connected();

void send_hello_to_server();

void send_board_move(int from_row, int from_col, int to_row, int to_col);

String get_virtual_move();

void clear_virtual_move();