#pragma once

#include <iostream>
#include <string>
#include <cctype>
#include "../board/board.hpp"

class ChessVisual
{
private:
    Board *board;

    const std::string PIECE_SYMBOLS[2][6] = {
        {"♚", "♛", "♜", "♝", "♞", "♟"},
        {"♔", "♕", "♖", "♗", "♘", "♙"}
    };

    int piece_to_index(PIECE_TYPE piece);
    PIECE_TYPE get_piece_at(int square);
    COLORS get_color_at(int square);
    MOVE_TYPE determine_move_type(Move &move, const std::string &input);

public:
    ChessVisual(Board *_board);
    
    void display();
    Move parse_move(const std::string &input);
    void play();
};