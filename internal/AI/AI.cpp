#include <bit>
#include "AI.hpp"
#include "./helpers.hpp"
#include "../shared/constants.hpp"
#include "../shared/utils.hpp"

#define DOUBLED_PAWN_PENALTY 20
#define SHIELD_MISSING_PENALTY 40
#define CASTLE_RIGHTS_BONUS 30

bitboard FILES[8] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};

AI::AI(Board *_board) : board(_board) {}

const int PIECE_VALUES[8] = {10000, 900, 500, 300, 300, 100, 0};

int AI::score_move(Move &move)
{
    if (move.get_type() == MOVE_TYPE::CAPTURE || move.get_type() == MOVE_TYPE::EN_PASSANT)
        return 10000 + PIECE_VALUES[(int)move.get_captured()] - ((int)move.get_piece());

    if (move.get_type() == MOVE_TYPE::PROMOTION)
        return 9000 + PIECE_VALUES[(int)move.get_promoutioned()];

    if (move.get_type() == MOVE_TYPE::CASTLE)
        return 1000;

    return 0;
}

Move AI::get_best_move(int depth)
{
    MoveList list = board->ugly_moves();
    COLORS color = board->get_turn_color();
    bool is_max = (color == COLORS::WHITE);

    int best_eval = is_max ? -1e6 : 1e6;
    Move best_move;

    int move_scores[256];
    for (int i = 0; i < list.count; ++i)
        move_scores[i] = score_move(list.moves[i]);

    for (int i = 0; i < list.count; ++i)
    {
        int best_index = i;
        for (int j = i + 1; j < list.count; ++j)
            if (move_scores[j] > move_scores[best_index])
                best_index = j;
        
        std::swap(list.moves[i], list.moves[best_index]);
        std::swap(move_scores[i], move_scores[best_index]);

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

    int move_scores[256];
    for (int i = 0; i < list.count; ++i)
        move_scores[i] = score_move(list.moves[i]);

    if (is_max)
    {
        int max_eval = -1e6;

        for (int i = 0; i < list.count; ++i)
        {
            int best_index = i;
            for (int j = i + 1; j < list.count; ++j)
                if (move_scores[j] > move_scores[best_index])
                    best_index = j;
            
            std::swap(list.moves[i], list.moves[best_index]);
            std::swap(move_scores[i], move_scores[best_index]);

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
            int best_index = i;
            for (int j = i + 1; j < list.count; ++j)
                if (move_scores[j] > move_scores[best_index])
                    best_index = j;
                    
            std::swap(list.moves[i], list.moves[best_index]);
            std::swap(move_scores[i], move_scores[best_index]);

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
    int total = 0;

    for (int i = 0; i < 64; i++)
    {
        auto [piece, color] = board->piece_at(i);
        total += evaluate_piece(piece, color, i);
    }

    total += evaluate_pawn_structure();
    total += evaluate_king_safety();

    return total;
}

int AI::evaluate_piece(PIECE_TYPE piece, COLORS color, int sq)
{

    if (piece == PIECE_TYPE::NONE)
        return 0;

    bool is_white = (color == COLORS::WHITE);
    int value;

    switch (piece)
    {
    case PIECE_TYPE::KING:
        value = 9000 + (is_white ? king_eval_white[sq] : king_eval_black[sq]);
        break;
    case PIECE_TYPE::QUEEN:
        value = 900 + queen_eval[sq];
        break;
    case PIECE_TYPE::ROOK:
        value = 500 + (is_white ? rook_eval_white[sq] : rook_eval_black[sq]);
        break;
    case PIECE_TYPE::BISHOP:
        value = 300 + (is_white ? bishop_eval_white[sq] : bishop_eval_black[sq]);
        break;
    case PIECE_TYPE::KNIGHT:
        value = 300 + knight_eval[sq];
        break;
    case PIECE_TYPE::PAWN:
        value = 100 + (is_white ? pawn_eval_white[sq] : pawn_eval_black[sq]);
        break;

    default:
        return 0;
    }

    return is_white ? value : -value;
}

int AI::evaluate_pawn_structure()
{
    int pawn_score = 0;

    for (int i = 0; i < 8; i++)
    {
        bitboard w_pawns_in_file = board->get_pawns(COLORS::WHITE) & FILES[i];
        bitboard b_pawns_in_file = board->get_pawns(COLORS::BLACK) & FILES[i];

        int w_count = std::__popcount(w_pawns_in_file);
        int b_count = std::__popcount(b_pawns_in_file);

        pawn_score -= (w_count - 1) * DOUBLED_PAWN_PENALTY;
        pawn_score += (b_count - 1) * DOUBLED_PAWN_PENALTY;
    }

    return pawn_score;
}

int AI::evaluate_king_safety()
{
    int safety_score = 0;

    uint8_t rights = board->get_castle_rights();

    bitboard w_king = board->get_king(COLORS::WHITE);
    int w_king_sq = bitScanForward(w_king);

    if (rights & 0b0011)
        safety_score += CASTLE_RIGHTS_BONUS;

    if (w_king_sq == 62 || w_king_sq == 63)
    {
        bitboard shield_mask = 0xE000ULL;
        int missing_pawns = 3 - std::__popcount(board->get_pawns(COLORS::WHITE) & shield_mask);
        safety_score -= missing_pawns * SHIELD_MISSING_PENALTY;
    }
    else if (w_king_sq == 58 || w_king_sq == 57)
    {
        bitboard shield_mask = 0x700ULL;
        int missing_pawns = 3 - std::__popcount(board->get_pawns(COLORS::WHITE) & shield_mask);
        safety_score -= missing_pawns * SHIELD_MISSING_PENALTY;
    }

    bitboard b_king = board->get_king(COLORS::BLACK);

    int b_king_sq = bitScanForward(b_king);

    if (rights & 0b1100)
        safety_score -= CASTLE_RIGHTS_BONUS;

    if (b_king_sq == 62 || b_king_sq == 63)
    {
        bitboard shield_mask = 0xE0000000000000ULL;
        int missing_pawns = 3 - std::__popcount(board->get_pawns(COLORS::BLACK) & shield_mask);
        safety_score += missing_pawns * SHIELD_MISSING_PENALTY;
    }
    else if (b_king_sq == 58 || b_king_sq == 57)
    {
        bitboard shield_mask = 0x7000000000000ULL;
        int missing_pawns = 3 - std::__popcount(board->get_pawns(COLORS::BLACK) & shield_mask);
        safety_score += missing_pawns * SHIELD_MISSING_PENALTY;
    }

    return safety_score;
}
