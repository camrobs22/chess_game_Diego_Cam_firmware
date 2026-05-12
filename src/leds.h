#include <Arduino.h>
#include <stdint.h>
#include <FastLED.h>

// LED decleration
#define NUM_LEDS 25
#define DIN1_PIN 13
#define DIN2_PIN 9
#define DIN3_PIN 10
#define DIN4_PIN 11

// define all the possible tiles
const int a1[4] = {0, 1, 8, 9};
const int a2[4] = {1, 2, 7, 8};
const int a3[4] = {2, 3, 6, 7};
const int a4[4] = {3, 4, 5, 6};
const int b4[4] = {5, 6, 13, 14};
const int b3[4] = {6, 7, 12, 13};
const int b2[4] = {7, 8, 11, 12};
const int b1[4] = {8, 9, 10, 11};
const int c1[4] = {10, 11, 18, 19};
const int c2[4] = {11, 12, 17, 18};
const int c3[4] = {12, 13, 16, 17};
const int c4[4] = {13, 14, 15, 16};
const int d4[4] = {15, 16, 23, 24};
const int d3[4] = {16, 17, 22, 23};
const int d2[4] = {17, 18, 21, 22};
const int d1[4] = {18, 19, 20, 21};

void leds_init();

void set_led(int idx);

void test_led();

void show_move(String move);