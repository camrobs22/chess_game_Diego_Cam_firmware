#include "hall_sensor.h"

void hall_init(){
  // hall sensor setup
  analogReadResolution(12); // 0–4095

  pinMode(HALL_PIN1, INPUT);
  pinMode(HALL_PIN2, INPUT);
  pinMode(HALL_PIN3, INPUT);
  pinMode(HALL_PIN4, INPUT);

  pinMode(ENABLE1, OUTPUT);
  pinMode(ENABLE2, OUTPUT);
  pinMode(ENABLE3, OUTPUT);
  pinMode(ENABLE4, OUTPUT);
  
  digitalWrite(ENABLE1, LOW);
  digitalWrite(ENABLE2, LOW);
  digitalWrite(ENABLE3, LOW);
  digitalWrite(ENABLE4, LOW);
}

void test_hall(){
  digitalWrite(ENABLE1, HIGH);
  int hall1 = analogRead(HALL_PIN1);
  delayMicroseconds(100);
  digitalWrite(ENABLE1, LOW);
  digitalWrite(ENABLE2, HIGH);
  int hall2 = analogRead(HALL_PIN2);
  delayMicroseconds(100);
  digitalWrite(ENABLE2, LOW);
  digitalWrite(ENABLE3, HIGH);
  int hall3 = analogRead(HALL_PIN3);
  delayMicroseconds(100);
  digitalWrite(ENABLE3, LOW);
  digitalWrite(ENABLE4, HIGH);
  int hall4 = analogRead(HALL_PIN4);
  delayMicroseconds(100);
  digitalWrite(ENABLE4, LOW);
  float hall1_volt = hall1/4095.0 * 3.3;
  float hall2_volt = hall2/4095.0 * 3.3;
  float hall3_volt = hall3/4095.0 * 3.3;
  float hall4_volt = hall4/4095.0 * 3.3;
  Serial.printf("H1=%f H2=%f H3=%f H4=%f\n", hall1_volt, hall2_volt, hall3_volt, hall4_volt);
}

// returns the voltages of a row of hall sensors in row_volts
void get_hall_volt(int row_idx, float row_volts[8]){
    if (row_idx == 0){
        digitalWrite(ENABLE1, HIGH);
        int hall1 = analogRead(HALL_PIN1);
        int hall2 = analogRead(HALL_PIN2);
        int hall3 = analogRead(HALL_PIN3);
        int hall4 = analogRead(HALL_PIN4);
        row_volts[0] = hall1/4095.0 * 3.3;
        row_volts[1] = hall2/4095.0 * 3.3;
        row_volts[2] = hall3/4095.0 * 3.3;
        row_volts[3] = hall4/4095.0 * 3.3;
        digitalWrite(ENABLE1, LOW);
        return;
    }
    else if (row_idx == 1){
        digitalWrite(ENABLE2, HIGH);
        int hall1 = analogRead(HALL_PIN1);
        int hall2 = analogRead(HALL_PIN2);
        int hall3 = analogRead(HALL_PIN3);
        int hall4 = analogRead(HALL_PIN4);
        row_volts[0] = hall1/4095.0 * 3.3;
        row_volts[1] = hall2/4095.0 * 3.3;
        row_volts[2] = hall3/4095.0 * 3.3;
        row_volts[3] = hall4/4095.0 * 3.3;
        digitalWrite(ENABLE2, LOW);
        return;
    }
    else if (row_idx == 2){
        digitalWrite(ENABLE3, HIGH);
        int hall1 = analogRead(HALL_PIN1);
        int hall2 = analogRead(HALL_PIN2);
        int hall3 = analogRead(HALL_PIN3);
        int hall4 = analogRead(HALL_PIN4);
        row_volts[0] = hall1/4095.0 * 3.3;
        row_volts[1] = hall2/4095.0 * 3.3;
        row_volts[2] = hall3/4095.0 * 3.3;
        row_volts[3] = hall4/4095.0 * 3.3;
        digitalWrite(ENABLE3, LOW);
        return;
    }
    else if (row_idx == 3){
        digitalWrite(ENABLE4, HIGH);
        int hall1 = analogRead(HALL_PIN1);
        int hall2 = analogRead(HALL_PIN2);
        int hall3 = analogRead(HALL_PIN3);
        int hall4 = analogRead(HALL_PIN4);
        row_volts[0] = hall1/4095.0 * 3.3;
        row_volts[1] = hall2/4095.0 * 3.3;
        row_volts[2] = hall3/4095.0 * 3.3;
        row_volts[3] = hall4/4095.0 * 3.3;
        digitalWrite(ENABLE4, LOW);
        return;
    }
    // else if (row_idx == 4){
    //     digitalWrite(ENABLE5, HIGH);
    //     int hall1 = analogRead(HALL_PIN1);
    //     int hall2 = analogRead(HALL_PIN2);
    //     int hall3 = analogRead(HALL_PIN3);
    //     int hall4 = analogRead(HALL_PIN4);
    //     row_volts[0] = hall1/4095.0 * 3.3;
    //     row_volts[1] = hall2/4095.0 * 3.3;
    //     row_volts[2] = hall3/4095.0 * 3.3;
    //     row_volts[3] = hall4/4095.0 * 3.3;
    //     digitalWrite(ENABLE5, LOW);
    //     return;
    // }
    // else if (row_idx == 5){
    //     digitalWrite(ENABLE6, HIGH);
    //     int hall1 = analogRead(HALL_PIN1);
    //     int hall2 = analogRead(HALL_PIN2);
    //     int hall3 = analogRead(HALL_PIN3);
    //     int hall4 = analogRead(HALL_PIN4);
    //     row_volts[0] = hall1/4095.0 * 3.3;
    //     row_volts[1] = hall2/4095.0 * 3.3;
    //     row_volts[2] = hall3/4095.0 * 3.3;
    //     row_volts[3] = hall4/4095.0 * 3.3;
    //     digitalWrite(ENABLE6, LOW);
    //     return;
    // }
    // else if (row_idx == 6){
    //     digitalWrite(ENABLE7, HIGH);
    //     int hall1 = analogRead(HALL_PIN1);
    //     int hall2 = analogRead(HALL_PIN2);
    //     int hall3 = analogRead(HALL_PIN3);
    //     int hall4 = analogRead(HALL_PIN4);
    //     row_volts[0] = hall1/4095.0 * 3.3;
    //     row_volts[1] = hall2/4095.0 * 3.3;
    //     row_volts[2] = hall3/4095.0 * 3.3;
    //     row_volts[3] = hall4/4095.0 * 3.3;
    //     digitalWrite(ENABLE7, LOW);
    //     return;
    // }
    // else if (row_idx == 7){
    //     digitalWrite(ENABLE8, HIGH);
    //     int hall1 = analogRead(HALL_PIN1);
    //     int hall2 = analogRead(HALL_PIN2);
    //     int hall3 = analogRead(HALL_PIN3);
    //     int hall4 = analogRead(HALL_PIN4);
    //     row_volts[0] = hall1/4095.0 * 3.3;
    //     row_volts[1] = hall2/4095.0 * 3.3;
    //     row_volts[2] = hall3/4095.0 * 3.3;
    //     row_volts[3] = hall4/4095.0 * 3.3;
    //     digitalWrite(ENABLE8, LOW);
    //     return;
    // }
    else{
        return;
    }
}
