#ifndef STATE_H
#define STATE_H
#include <Arduino.h>
#include <stdint.h>
#include "hall_sensor.h"

enum PieceType {
    EMPTY,
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
};

enum PieceColor {
    WHITE,
    BLACK,
    NONE
};

struct ChessPiece {
    PieceColor color;
    PieceType piecetype;
};

struct BoardState{
    ChessPiece cur_state[8][8];
    ChessPiece past_state[8][8];
};

// global variable that used to measure current state
extern BoardState GameState; 

// uses analog voltage to see what chess piece type it is
ChessPiece get_chess_piece_type(float voltage);

// initializes the state 
void setup_state();

// returns pollFlag --> pollFlag set to true every __ ms 
bool ready_for_state_update();


// updates the global gamestate variable
void update_state();

// returns gamestate
BoardState get_state();

// checks to see if a valid move
bool valid_game_update();

#endif