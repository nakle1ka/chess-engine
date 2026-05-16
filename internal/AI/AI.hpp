#pragma once
#include "../board/board.hpp"
#include "../shared/move.hpp"
#include "../shared/types.hpp"

class AI
{
private:
    Board *board;

public:
    AI(Board *_board);

    
    Move get_best_move(int depth);
    int evaluate_board();
    
    private:
    int score_move(Move &move);
    int minimax(int depth, int alpha, int beta, bool is_max);

    int evaluate_piece(PIECE_TYPE type, COLORS color, int sq);
    int evaluate_pawn_structure();
    int evaluate_king_safety();
};