#include "hall_sensor.h"

// take multiple readings to help filter out hall sensor noise
float get_average_hall_voltage(int pin){
  analogRead(pin);

  int total = 0;
  for (int i = 0; i < 3; i++){
    total += analogRead(pin);
    delayMicroseconds(20);
  }

  float average = total / 3.0f;
  return average / 4095.0f * 3.3f;
}

void hall_init(){
  // hall sensor setup
  analogReadResolution(12); // 0–4095

  pinMode(HALL_PIN1, INPUT);
  pinMode(HALL_PIN2, INPUT);
  pinMode(HALL_PIN3, INPUT);
  pinMode(HALL_PIN4, INPUT);
  pinMode(HALL_PIN5, INPUT);
  pinMode(HALL_PIN6, INPUT);
  pinMode(HALL_PIN7, INPUT);
  pinMode(HALL_PIN8, INPUT);


  pinMode(ENABLE1, OUTPUT);
  pinMode(ENABLE2, OUTPUT);
  pinMode(ENABLE3, OUTPUT);
  pinMode(ENABLE4, OUTPUT);
  pinMode(ENABLE5, OUTPUT);
  pinMode(ENABLE6, OUTPUT);
  pinMode(ENABLE7, OUTPUT);
  pinMode(ENABLE8, OUTPUT);
  
  digitalWrite(ENABLE1, LOW);
  digitalWrite(ENABLE2, LOW);
  digitalWrite(ENABLE3, LOW);
  digitalWrite(ENABLE4, LOW);
  digitalWrite(ENABLE5, LOW);
  digitalWrite(ENABLE6, LOW);
  digitalWrite(ENABLE7, LOW);
  digitalWrite(ENABLE8, LOW);
}

void test_hall(){
  const uint8_t enablePins[8] = {
    ENABLE1, ENABLE2, ENABLE3, ENABLE4,
    ENABLE5, ENABLE6, ENABLE7, ENABLE8
  };
  const uint8_t hallPins[8] = {
    HALL_PIN1, HALL_PIN2, HALL_PIN3, HALL_PIN4,
    HALL_PIN5, HALL_PIN6, HALL_PIN7, HALL_PIN8
  };

  Serial.println("Hall voltages (rows EN1-EN8, columns OUT1-OUT8):");

  for (int row = 0; row < 8; row++) {
    digitalWrite(enablePins[row], HIGH);
    delayMicroseconds(300);

    Serial.printf("EN%d:", row + 1);
    for (int col = 0; col < 8; col++) {
      float voltage = get_average_hall_voltage(hallPins[col]);
      Serial.printf(" %.3f", voltage);
    }
    Serial.println();

    digitalWrite(enablePins[row], LOW);
  }

  Serial.println();
}

// returns the voltages of a row of hall sensors in row_volts
void get_hall_volt(int row_idx, float row_volts[8]){
  const uint8_t enablePins[8] = {
    ENABLE1, ENABLE2, ENABLE3, ENABLE4,
    ENABLE5, ENABLE6, ENABLE7, ENABLE8
  };
  const uint8_t hallPins[8] = {
    HALL_PIN1, HALL_PIN2, HALL_PIN3, HALL_PIN4,
    HALL_PIN5, HALL_PIN6, HALL_PIN7, HALL_PIN8
  };

  if (row_idx < 0 || row_idx >= 8) {
    return;
  }

  digitalWrite(enablePins[row_idx], HIGH);
  delayMicroseconds(300);

  for (int col = 0; col < 8; col++) {
    row_volts[col] = get_average_hall_voltage(hallPins[col]);
  }

  digitalWrite(enablePins[row_idx], LOW);
}
