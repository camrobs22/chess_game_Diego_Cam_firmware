#include "state.h"
#include "hall_sensor.h"


// setup for hardware timer used for interrupt
hw_timer_t *timer = nullptr;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
// flag updated in interrupt and idx
static volatile bool pollFlag = false;
static volatile int row_idx;
static volatile bool full_scan_complete = false;
static PieceType candidate_state[8][8];
static uint8_t candidate_streak[8][8];
static constexpr uint8_t REQUIRED_STABLE_ROW_SCANS = 2;

BoardState GameState;

int total_pieces_white = 16;
int total_pieces_black = 16;

static void update_expected_piece_totals() {
    total_pieces_white = 0;
    total_pieces_black = 0;

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (GameState.cur_state[i][j].piecetype == WHITE){
                total_pieces_white++;
            }
            else if (GameState.cur_state[i][j].piecetype == BLACK){
                total_pieces_black++;
            }
        }
    }
}

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  pollFlag = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}



ChessPiece get_chess_piece_type(float voltage){
    // bunch of if statements for test voltages
    ChessPiece chesspiece;
    if (voltage <= 1.55){
        chesspiece.piecetype = BLACK;
    }
    else if (voltage >= 1.74){
        chesspiece.piecetype = WHITE;
    }
    else{
        chesspiece.piecetype = EMPTY;
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
            candidate_state[i][j] = GameState.cur_state[i][j].piecetype;
            candidate_streak[i][j] = 0;
        }
    }
    // set curstate = past state
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            GameState.past_state[i][j] = GameState.cur_state[i][j];
        }
    }
    update_expected_piece_totals();

    //print the total number of white and black pieces
    int total_pieces_white_check = 0;
    int total_pieces_black_check = 0;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (GameState.cur_state[i][j].piecetype == WHITE){
                total_pieces_white_check++;
            }
            else if (GameState.cur_state[i][j].piecetype == BLACK){
                total_pieces_black_check++;
            }
        }
    }
    Serial.printf("num white pieces = %i and num black pieces = %i\r\n", total_pieces_white_check, total_pieces_black_check);
    Serial.printf("expected white pieces = %i and expected black pieces = %i\r\n", total_pieces_white, total_pieces_black);

    // now we have gone through more slowly through intial placement enable interrupt to start getting updates automatically
    timer = timerBegin(0, 80, true);        // 1 tick = 1 µs
    timerAttachInterrupt(timer, &onTimer, true);

    timerAlarmWrite(timer, 100000, true);    // 64 Hz

    timerAlarmEnable(timer);

    Serial.printf("Playing game with %i white pieces and %i black pieces\r\n", total_pieces_white, total_pieces_black);

    return;
}


bool ready_for_state_update(){
    bool shouldPoll = false;

    portENTER_CRITICAL(&timerMux);
    if (pollFlag) {
        shouldPoll = true;
        pollFlag = false;
    }
    portEXIT_CRITICAL(&timerMux);

    return shouldPoll;
}


void update_state(){
    float volts[8];
    get_hall_volt(row_idx, volts);
    for (int j = 0; j < 8; j++){
        PieceType observed = get_chess_piece_type(volts[j]).piecetype;
        PieceType current = GameState.cur_state[row_idx][j].piecetype;

        if (observed == current){
            candidate_state[row_idx][j] = current;
            candidate_streak[row_idx][j] = 0;
        }
        else if (observed == candidate_state[row_idx][j]){
            if (candidate_streak[row_idx][j] < REQUIRED_STABLE_ROW_SCANS){
                candidate_streak[row_idx][j]++;
            }

            if (candidate_streak[row_idx][j] >= REQUIRED_STABLE_ROW_SCANS){
                GameState.cur_state[row_idx][j].piecetype = observed;
                candidate_streak[row_idx][j] = 0;
            }
        }
        else{
            candidate_state[row_idx][j] = observed;
            candidate_streak[row_idx][j] = 1;
        }
    }
    
    row_idx++;

    if (row_idx >= 8){
        row_idx = 0;
        full_scan_complete = true;
    }
    return;
}

void commit_state(){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            GameState.past_state[i][j] = GameState.cur_state[i][j];
        }
    }

    update_expected_piece_totals();
}

bool board_scan_complete(){
    bool complete = false;

    portENTER_CRITICAL(&timerMux);
    if (full_scan_complete) {
        complete = true;
        full_scan_complete = false;
    }
    portEXIT_CRITICAL(&timerMux);

    return complete;
}

// checks all criteria to make sure a game state update was valid
bool valid_game_update(){
    // types of valid moves: total pieces at end equals beginning and game states not the same, or total pieces is different but a different color piece took the place of another piece
    int total_pieces_white_check = 0;
    int total_pieces_black_check = 0;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (GameState.cur_state[i][j].piecetype != EMPTY && GameState.cur_state[i][j].piecetype == WHITE){
                total_pieces_white_check++;
            }
            else if (GameState.cur_state[i][j].piecetype != EMPTY && GameState.cur_state[i][j].piecetype == BLACK){
                total_pieces_black_check++;
            }
        }
    }
    Serial.printf("num white pieces = %i and num black pieces = %i\r\n", total_pieces_white_check, total_pieces_black_check);
    Serial.printf("expected white pieces = %i and expected black pieces = %i\r\n", total_pieces_white, total_pieces_black);
    // return true if total pieces the same, that means the chess piece has been picked up and moved to another place without taking an opposing piece
    if (total_pieces_white == total_pieces_white_check && total_pieces_black == total_pieces_black_check){
        return true;
    }
    // black piece took a white piece
    else if (total_pieces_white - 1 == total_pieces_white_check && total_pieces_black == total_pieces_black_check){
        // loop through all previous white piece positions and see if they got replaced by a black piece
        for (int i = 0; i < 8; i++){
            for (int j = 0; j < 8; j++){
                if (GameState.past_state[i][j].piecetype == WHITE && GameState.cur_state[i][j].piecetype == BLACK){
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
                if (GameState.past_state[i][j].piecetype == BLACK && GameState.cur_state[i][j].piecetype == WHITE){
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
