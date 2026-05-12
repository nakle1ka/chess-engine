#include <iostream>
#include <string>
#include "../internal/board/board.hpp"
#include "../internal/rays/rays.hpp"
#include "../internal/chess_visual/chess_visual.hpp"

int main()
{
    system("chcp 65001");
    Rays rays;
    Attacks attacks(&rays);
    Board board(&attacks); 
    board.set_default();
    // board.add_piece(4, COLORS::WHITE, PIECE_TYPE::KING);
    // board.add_piece(0, COLORS::WHITE, PIECE_TYPE::ROOK);
    // board.add_piece(63, COLORS::BLACK, PIECE_TYPE::KING);
    ChessVisual game(&board);
    game.play();

    return 0;
}