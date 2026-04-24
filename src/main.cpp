#include <Arduino.h>
#include <stdint.h>
#include "hall_sensor.h"
#include "leds.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  hall_init();

}

void loop() {
  // put your main code here, to run repeatedly:
  test_hall();
  set_led(0);
  delay(100);
}
