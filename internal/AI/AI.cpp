#include "AI.hpp"
#include "./helpers.hpp"

AI::AI(Board *_board) : board(_board) {}

Move AI::get_best_move(int depth)
{
    MoveList list = board->ugly_moves();
    COLORS color = board->get_turn_color();
    bool is_max = (color == COLORS::WHITE);

    int best_eval = is_max ? -1e6 : 1e6;
    Move best_move;

    for (int i = 0; i < list.count; ++i)
    {
        Move move = list.moves[i];
        board->ugly_move(move);
        if (board->is_king_checked(color))
        {
            board->undo_move();
            continue;
        }

        int eval = minimax(depth - 1, -1e6, 1e6, !is_max);
        board->undo_move();
        if ((is_max && eval > best_eval) || (!is_max && eval < best_eval))
        {
            best_eval = eval;
            best_move = move;
        }
    }

    return best_move;
}

int AI::minimax(int depth, int alpha, int beta, bool is_max)
{
    if (depth == 0)
        return evaluate_board();

    MoveList list = board->ugly_moves();
    int legal_moves_count = 0;
    COLORS color = board->get_turn_color();

    if (is_max)
    {
        int max_eval = -1e6;

        for (int i = 0; i < list.count; ++i)
        {
            Move move = list.moves[i];
            board->ugly_move(move);
            if (board->is_king_checked(color))
            {
                board->undo_move();
                continue;
            }
            ++legal_moves_count;

            int eval = minimax(depth - 1, alpha, beta, !is_max);
            board->undo_move();
            max_eval = std::max(eval, max_eval);
            alpha = std::max(eval, alpha);

            if (alpha >= beta)
                break;
        }

        if (legal_moves_count == 0)
            return board->is_king_checked(color) ? -1e6 + 100 - depth : 0;

        return max_eval;
    }
    else
    {
        int min_eval = 1e6;

        for (int i = 0; i < list.count; ++i)
        {
            Move move = list.moves[i];
            board->ugly_move(move);
            if (board->is_king_checked(color))
            {
                board->undo_move();
                continue;
            }
            ++legal_moves_count;

            int eval = minimax(depth - 1, alpha, beta, !is_max);
            board->undo_move();
            min_eval = std::min(eval, min_eval);
            beta = std::min(eval, beta);

            if (alpha >= beta)
                break;
        }

        if (legal_moves_count == 0)
            return board->is_king_checked(color) ? 1e6 - 100 + depth : 0;

        return min_eval;
    }
}

int AI::evaluate_board()
{
    double total = 0;

    for (int i = 0; i < 64; i++)
    {
        auto [piece, color] = board->piece_at(i);
        total += evaluate_piece(piece, color, i);
    }

    return total;
}

int AI::evaluate_piece(PIECE_TYPE piece, COLORS color, int sq)
{

    if (piece == PIECE_TYPE::NONE)
        return 0;

    bool is_white = (color == COLORS::WHITE);
    double value;

    switch (piece)
    {
    case PIECE_TYPE::KING:
        value = 900 + (is_white ? king_eval_white[sq] : king_eval_black[sq]);
        break;
    case PIECE_TYPE::QUEEN:
        value = 90 + queen_eval[sq];
        break;
    case PIECE_TYPE::ROOK:
        value = 50 + (is_white ? rook_eval_white[sq] : rook_eval_black[sq]);
        break;
    case PIECE_TYPE::BISHOP:
        value = 30 + (is_white ? bishop_eval_white[sq] : bishop_eval_black[sq]);
        break;
    case PIECE_TYPE::KNIGHT:
        value = 30 + knight_eval[sq];
        break;
    case PIECE_TYPE::PAWN:
        value = 10 + (is_white ? pawn_eval_white[sq] : pawn_eval_black[sq]);
        break;

    default:
        return 0;
    }

    return is_white ? value : -value;
}
