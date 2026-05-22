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

  if (idx >= 0 && idx < NUM_LEDS) {
    ledBoards[board_idx][idx] = CRGB::Blue;
  }

  FastLED.show();
  return;
}

void test_led(){
  for (int board = 0; board < 4; board++){
    for (int i = 0; i < 25; i++){
      FastLED.clear();
      ledBoards[board][i] = CRGB::Red;
      delay(200);
      FastLED.show();
    }
  }
  return;
}


void light_tile(String tile) {
  int board_idx;
  int col_idx;
  int row_idx;

  bool valid_tile = get_tile_location(tile, board_idx, row_idx, col_idx);

  if (!valid_tile){
    Serial.print("not a valid tile\r\n");
    Serial.println(tile);
    return;
  }

  // turn on the tile by finding the 4 led indxs to turn on
  for (int i = 0; i < 4; i++){
    // get the index of the led to turn on
    int led_idx = tile_leds[row_idx][col_idx][i];
    //set led on
    ledBoards[board_idx][led_idx] = CRGB::Blue;
  }

}

void show_move(String move){

  // combine first two to get first tile to light up
  String tile1 = move.substring(0, 2);

  String tile2 = move.substring(2, 4);
  FastLED.clear();
  light_tile(tile1);
  light_tile(tile2);
  FastLED.show();
  return;
}

void clear_leds(){
  FastLED.clear();
  FastLED.show();
  return;
}