#include "state.h"
#include "hall_sensor.h"

static volatile int row_idx;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  pollFlag = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}



PieceType get_chess_piece_type(float voltage){
    // bunch of if statements for test voltages
    if (voltage == 0){
        return EMPTY;
    }
    // need to finish when testing
    //default return empty
    return EMPTY;
}

// reads all of the sensors initially to get game state --> manually goes through each row 
void setup_state(){
    row_idx = 0;
    pollFlag = false;
    // initally read all of board without interrupts enabled to get a more accurate and slow first read
    for (int i = 0; i < 8; i++){
        float volts[8];
        get_hall_volt(i, volts);
        for (int j = 0; j < 8; j++){
            GameState.cur_state[i][j] = get_chess_piece_type(volts[j]);
        }
    }
    // set curstate = past state
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            GameState.past_state[i][j] = GameState.cur_state[i][j];
        }
    }
    // now we have gone through more slowly through intial placement enable interrupt to start getting updates automatically
    timer = timerBegin(0, 80, true);        // 1 tick = 1 µs
    timerAttachInterrupt(timer, &onTimer, true);

    timerAlarmWrite(timer, 15625, true);    // 64 Hz

    timerAlarmEnable(timer);

    return;
}