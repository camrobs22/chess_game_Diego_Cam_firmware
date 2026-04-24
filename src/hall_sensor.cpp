#include "hall_sensor.h"

void hall_init(){
  // hall sensor setup
  analogReadResolution(12); // 0–4095

  pinMode(HALL_PIN1, INPUT);
  pinMode(HALL_PIN2, INPUT);
  pinMode(HALL_PIN3, INPUT);
  pinMode(HALL_PIN4, INPUT);
}

void test_hall(){
  int hall1 = analogRead(HALL_PIN1);
  int hall2 = analogRead(HALL_PIN2);
  int hall3 = analogRead(HALL_PIN3);
  int hall4 = analogRead(HALL_PIN4);
  float hall1_volt = hall1/4095.0 * 3.3;
  float hall2_volt = hall2/4095.0 * 3.3;
  float hall3_volt = hall3/4095.0 * 3.3;
  float hall4_volt = hall4/4095.0 * 3.3;
  Serial.printf("H1=%f H2=%f H3=%f H4=%f\n", hall1_volt, hall2_volt, hall3_volt, hall4_volt);
}


