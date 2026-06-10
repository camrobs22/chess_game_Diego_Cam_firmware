#include <Arduino.h>
#include <stdint.h>

// Hall sensor outputs. Board connections 5-8 are physically reversed.
#define HALL_PIN1 1
#define HALL_PIN2 2
#define HALL_PIN3 4
#define HALL_PIN4 5
#define HALL_PIN5 6
#define HALL_PIN6 7
#define HALL_PIN7 8
#define HALL_PIN8 9

#define ENABLE1 11
#define ENABLE2 12
#define ENABLE3 13
#define ENABLE4 14
#define ENABLE5 18
#define ENABLE6 17
#define ENABLE7 16
#define ENABLE8 15

void hall_init();

void test_hall();

void get_hall_volt(int row_idx, float row_volts[8]);
