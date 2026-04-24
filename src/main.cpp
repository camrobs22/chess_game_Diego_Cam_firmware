#include <Arduino.h>
#include <stdint.h>
#include <FastLED.h>
// hall sensor delcerations --> have to use channel 1 
#define HALL_PIN1 36
#define HALL_PIN2 39
#define HALL_PIN3 34
#define HALL_PIN4 35
// LED decleration
#define NUM_LEDS 25
#define DIN_PIN 13
CRGB leds[NUM_LEDS];

// put function declarations here:

//

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // hall sensor setup
  analogReadResolution(12); // 0–4095

  pinMode(HALL_PIN1, INPUT);
  pinMode(HALL_PIN2, INPUT);
  pinMode(HALL_PIN3, INPUT);
  pinMode(HALL_PIN4, INPUT);


  // led chain setup
  FastLED.addLeds<WS2812, DIN_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  int hall1 = analogRead(HALL_PIN1);
  int hall2 = analogRead(HALL_PIN2);
  int hall3 = analogRead(HALL_PIN3);
  int hall4 = analogRead(HALL_PIN4);
  float hall1_volt = hall1/4095.0 * 3.3;
  float hall2_volt = hall2/4095.0 * 3.3;
  float hall3_volt = hall3/4095.0 * 3.3;
  float hall4_volt = hall4/4095.0 * 3.3;
  Serial.printf("H1=%f H2=%f H3=%f H4=%f\n", hall1_volt, hall2_volt, hall3_volt, hall4_volt);

  delay(100);
}
