#include <Arduino.h>
#include <stdint.h>

// hall sensor delcerations --> have to use channel 1 
#define HALL_PIN1 36
#define HALL_PIN2 39
#define HALL_PIN3 34
#define HALL_PIN4 35

void hall_init();

void test_hall();