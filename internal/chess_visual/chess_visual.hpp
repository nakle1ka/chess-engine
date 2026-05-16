#pragma once

#include <iostream>
#include <string>
#include <cctype>
#include "../board/board.hpp"
#include "../AI/AI.hpp"

class ChessVisual
{
private:
    Board *board;
    AI *ai;

    const std::string PIECE_SYMBOLS[2][6] = {
        {"♚", "♛", "♜", "♝", "♞", "♟"},
        {"♔", "♕", "♖", "♗", "♘", "♙"}};

    int piece_to_index(PIECE_TYPE piece);
    MOVE_TYPE determine_move_type(Move &move, const std::string &input);

public:
    ChessVisual(Board *_board, AI *_ai);

    void display(bool flip = false);
    void print_square(int square, int rank, int file);
    Move parse_move(const std::string &input);
    void play();
};