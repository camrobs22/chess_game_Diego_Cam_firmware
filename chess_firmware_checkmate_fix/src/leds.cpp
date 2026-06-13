#include "leds.h"

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];

CRGB* ledBoards[4] = {
  leds1,
  leds2,
  leds3,
  leds4
};

// LED positions within one quarter-board
const int tile_leds[4][4][4] = {
  // row 0
  {
    {0, 1, 8, 9},
    {1, 2, 7, 8},
    {2, 3, 6, 7},
    {3, 4, 5, 6}
  },
  // row 1
  {
    {8, 9, 10, 11},
    {7, 8, 11, 12},
    {6, 7, 12, 13},
    {5, 6, 13, 14}
  },
  // row 2
  {
    {10, 11, 18, 19},
    {11, 12, 17, 18},
    {12, 13, 16, 17},
    {13, 14, 15, 16}
  },
  // row 3
  {
    {18, 19, 20, 21},
    {17, 18, 21, 22},
    {16, 17, 22, 23},
    {15, 16, 23, 24}
  }
};

void leds_init() {
  FastLED.addLeds<WS2812, DIN1_PIN, GRB>(leds1, NUM_LEDS);
  FastLED.addLeds<WS2812, DIN2_PIN, GRB>(leds2, NUM_LEDS);
  FastLED.addLeds<WS2812, DIN3_PIN, GRB>(leds3, NUM_LEDS);
  FastLED.addLeds<WS2812, DIN4_PIN, GRB>(leds4, NUM_LEDS);

  FastLED.setBrightness(100);
  FastLED.clear();
  FastLED.show();
}

bool get_tile_location(String tile, int &boardIndex, int &localRow, int &localCol) {
  // checks for valid input like a4, c7, not ab43
  if (tile.length() != 2){
    return false;
  }

  char file = tile.charAt(0); // a-h
  char rank = tile.charAt(1); // 1-8
  // checks for validation
  if (file < 'a' || file > 'h'){
    return false;
  }
  if (rank < '1' || rank > '8'){
    return false;
  }

  int globalCol = file - 'a';      // a=0, b=1, ... h=7 --> leds start left to right
  int globalRow = '8' - rank;      // rank 8 = row 0 for leds , rank 1 = row 7 for leds

  bool rightSide = globalCol >= 4;
  bool bottomSide = globalRow >= 4;

  if (!rightSide && !bottomSide) {
    boardIndex = 0; // a8-d5
  }
  else if (rightSide && !bottomSide) {
    boardIndex = 1; // e8-h5
  }
  else if (!rightSide && bottomSide) {
    boardIndex = 2; // a4-d1
  }
  else {
    boardIndex = 3; // e4-h1
  }

  localCol = globalCol % 4;
  localRow = globalRow % 4;

  return true;
}

void set_led(int idx, int board_idx){
  FastLED.clear();

  if (board_idx >= 0 && board_idx < 4 && idx >= 0 && idx < NUM_LEDS) {
    ledBoards[board_idx][idx] = CRGB::Blue;
  }

  FastLED.show();
  return;
}

void test_led(){
  for (int board = 0; board < 4; board++){
    Serial.printf("Testing LED chain %d\n", board + 1);

    for (int i = 0; i < NUM_LEDS; i++){
      FastLED.clear();
      ledBoards[board][i] = CRGB::Red;
      FastLED.show();
      delay(200);
    }
  }

  FastLED.clear();
  FastLED.show();
  Serial.println("LED test complete");
}

static void fill_all_boards(const CRGB& color) {
  for (int board = 0; board < 4; board++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      ledBoards[board][i] = color;
    }
  }
}

static void flash_all(const CRGB& color, int flashes, int onMs, int offMs) {
  for (int i = 0; i < flashes; i++) {
    FastLED.clear();
    fill_all_boards(color);
    FastLED.show();
    delay(onMs);

    FastLED.clear();
    FastLED.show();
    delay(offMs);
  }
}

void light_tile(String tile) {
  light_tile(tile, CRGB::Blue);
}

void light_tile(String tile, const CRGB& color) {
  int board_idx;
  int col_idx;
  int row_idx;

  bool valid_tile = get_tile_location(tile, board_idx, row_idx, col_idx);

  if (!valid_tile){
    Serial.print("not a valid tile\r\n");
    Serial.println(tile);
    return;
  }

  // turn on the tile by finding the 4 led indexes to turn on
  for (int i = 0; i < 4; i++){
    int led_idx = tile_leds[row_idx][col_idx][i];
    ledBoards[board_idx][led_idx] = color;
  }
}

void show_move(String move){
  if (move.length() < 4) {
    Serial.println("show_move received invalid move string");
    return;
  }

  String tile1 = move.substring(0, 2);
  String tile2 = move.substring(2, 4);

  FastLED.clear();
  light_tile(tile1, CRGB::Blue);
  light_tile(tile2, CRGB::Blue);
  FastLED.show();
}

void show_capture_move(String move){
  if (move.length() < 4) {
    Serial.println("show_capture_move received invalid move string");
    return;
  }

  String fromTile = move.substring(0, 2);
  String toTile = move.substring(2, 4);

  // Blue = moving piece source, Red/Orange = captured-piece destination.
  FastLED.clear();
  light_tile(fromTile, CRGB::Blue);
  light_tile(toTile, CRGB::Red);
  FastLED.show();
  delay(250);

  FastLED.clear();
  light_tile(fromTile, CRGB::Blue);
  light_tile(toTile, CRGB::Orange);
  FastLED.show();
  delay(250);

  FastLED.clear();
  light_tile(fromTile, CRGB::Blue);
  light_tile(toTile, CRGB::Red);
  FastLED.show();
}

void show_castling_move(String move){
  if (move != "e1g1" && move != "e1c1" &&
      move != "e8g8" && move != "e8c8") {
    Serial.println("show_castling_move received a non-castling move");
    return;
  }

  const char rank = move.charAt(1);
  const bool kingSide = move.charAt(2) == 'g';
  const char rookFromFile = kingSide ? 'h' : 'a';
  const char rookToFile = kingSide ? 'f' : 'd';
  const int direction = kingSide ? 1 : -1;
  const int pathLength = kingSide ? 4 : 5;

  // castling animation
  for (int step = 0; step < pathLength; step++) {
    FastLED.clear();

    for (int pathIndex = 0; pathIndex <= step; pathIndex++) {
      String tile = "";
      tile += static_cast<char>('e' + pathIndex * direction);
      tile += rank;
      light_tile(tile, pathIndex == 0 ? CRGB::Blue : CRGB::Aqua);
    }

    FastLED.show();
    delay(110);
  }

  String kingFrom = "";
  kingFrom += 'e';
  kingFrom += rank;

  String kingTo = "";
  kingTo += kingSide ? 'g' : 'c';
  kingTo += rank;

  String rookFrom = "";
  rookFrom += rookFromFile;
  rookFrom += rank;

  String rookTo = "";
  rookTo += rookToFile;
  rookTo += rank;

  // keep the castling squares on
  FastLED.clear();
  light_tile(kingFrom, CRGB::Blue);
  light_tile(kingTo, CRGB::Aqua);
  light_tile(rookFrom, CRGB::Purple);
  light_tile(rookTo, CRGB::Gold);
  FastLED.show();
}

void show_invalid_move(){
  Serial.println("Showing invalid move LEDs");
  // invalid move animation
  flash_all(CRGB::Red, 2, 180, 120);
}

void show_win_animation(){
  // win animation
  FastLED.clear();
  for (int pass = 0; pass < 2; pass++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      for (int board = 0; board < 4; board++) {
        ledBoards[board][i] = CRGB::Green;
      }
      FastLED.show();
      delay(25);
    }
    FastLED.clear();
    FastLED.show();
    delay(100);
  }

  fill_all_boards(CRGB::Green);
  FastLED.show();
}

void show_loss_animation(){
  // loss animation
  for (int i = 0; i < 3; i++) {
    FastLED.clear();
    fill_all_boards(CRGB::Red);
    FastLED.show();
    delay(350);

    FastLED.clear();
    FastLED.show();
    delay(250);
  }

  fill_all_boards(CRGB::DarkRed);
  FastLED.show();
}

void show_draw_animation(){
  // draw animation
  for (int i = 0; i < 4; i++) {
    FastLED.clear();
    fill_all_boards(i % 2 == 0 ? CRGB::Yellow : CRGB::Purple);
    FastLED.show();
    delay(250);
  }

  FastLED.clear();
  FastLED.show();
}

void clear_leds(){
  FastLED.clear();
  FastLED.show();
  return;
}
