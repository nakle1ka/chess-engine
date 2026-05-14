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
    Board board(&attacks); board.set_default();
    AI ai(&board);
    ChessVisual game(&board, &ai); game.play();

    return 0;
}