#include "state.h"
#include "hall_sensor.h"


// setup for hardware timer used for interrupt
hw_timer_t *timer = nullptr;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
// flag updated in interrupt and idx
static volatile bool pollFlag = false;
static volatile int row_idx;

BoardState GameState;

int total_pieces_white = 16;
int total_pieces_black = 16;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  pollFlag = true;
  if (row_idx == 8){
    row_idx = 0;
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}



ChessPiece get_chess_piece_type(float voltage){
    // bunch of if statements for test voltages
    ChessPiece chesspiece;
    if (voltage < 0){
        chesspiece.color = WHITE;
    }
    else if (voltage > 0){
        chesspiece.color = BLACK;
    }
    if (voltage == 0){
        chesspiece.piecetype = EMPTY;
        chesspiece.color = NONE;
    }
    // need to finish when testing
    //default return empty
    return chesspiece;
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

bool ready_for_state_update(){
    return pollFlag;
}

void update_state(){
    // set prev state equal to cur state
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            GameState.past_state[i][j] = GameState.cur_state[i][j];
        }
    }
    float volts[8];
    get_hall_volt(row_idx, volts);
    for (int j = 0; j < 8; j++){
        GameState.cur_state[row_idx][j] = get_chess_piece_type(volts[j]);
    }
    row_idx++;
    return;
}
// checks all criteria to make sure a game state update was valid
bool valid_game_update(){
    // types of valid moves: total pieces at end equals beginning and game states not the same, or total pieces is different but a different color piece took the place of another piece
    int total_pieces_white_check = 0;
    int total_pieces_black_check = 0;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (GameState.cur_state[i][j].piecetype != EMPTY && GameState.cur_state[i][j].color == WHITE){
                total_pieces_white_check++;
            }
            else if (GameState.cur_state[i][j].piecetype != EMPTY && GameState.cur_state[i][j].color == BLACK){
                total_pieces_black_check++;
            }
        }
    }

    // return true if total pieces the same, that means the chess piece has been picked up and moved to another place without taking an opposing piece
    if (total_pieces_white == total_pieces_white_check && total_pieces_black == total_pieces_black_check){
        return true;
    }
    // black piece took a white piece
    else if (total_pieces_white - 1 == total_pieces_white_check && total_pieces_black == total_pieces_black_check){
        // loop through all previous white piece positions and see if they got replaced by a black piece
        for (int i = 0; i < 8; i++){
            for (int j = 0; j < 8; j++){
                if (GameState.past_state[i][j].color == WHITE && GameState.cur_state[i][j].color == BLACK){
                    total_pieces_white--;
                    return true;
                }
            }
        }
        // no pieces were taken
        return false;
        
    } 
    // white piece tok a black piece
    else if (total_pieces_black - 1 == total_pieces_black_check && total_pieces_white == total_pieces_white_check){
        // loop through all previous black piece positions and see if they got replaced by a white piece
        for (int i = 0; i < 8; i++){
            for (int j = 0; j < 8; j++){
                if (GameState.past_state[i][j].color == BLACK && GameState.cur_state[i][j].color == WHITE){
                    total_pieces_black--;
                    return true;
                }
            }
        }
        // no pieces were taken
        return false;
    }
    // else return false
    else{
        return false;
    }

}