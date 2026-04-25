#include <Arduino.h>
#include <stdint.h>

// hall sensor delcerations --> have to use channel 1 
#define HALL_PIN1 36
#define HALL_PIN2 39
#define HALL_PIN3 34
#define HALL_PIN4 35

#define ENABLE1 26
#define ENABLE2 27
#define ENABLE3 14
#define ENABLE4 12

void hall_init();

void test_hall();

void get_hall_volt(int row_idx, float row_volts[8]);