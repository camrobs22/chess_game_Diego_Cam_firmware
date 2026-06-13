#include <Arduino.h>
#include <stdint.h>
#include <FastLED.h>

// LED decleration
#define NUM_LEDS 25
#define DIN1_PIN 21
#define DIN3_PIN 39
#define DIN2_PIN 38
#define DIN4_PIN 40


void leds_init();

bool get_tile_location(String tile, int &boardIndex, int &localRow, int &localCol);

void set_led(int idx, int board_idx = 0);

void test_led();

void light_tile(String tile);
void light_tile(String tile, const CRGB& color);

void show_move(String move);
void show_capture_move(String move);
void show_castling_move(String move);
void show_invalid_move();
void show_win_animation();
void show_loss_animation();
void show_draw_animation();

void clear_leds();
