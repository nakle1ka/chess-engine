#include <tuple>
#include "./board.hpp"
#include "../shared/types.hpp"
#include "../shared/constants.hpp"
#include "../shared/errors.hpp"
#include "../shared/utils.hpp"

// from-to position, mask, rights index
std::tuple<int, int, bitboard, int> castles[] = {
    {4, 2, WHITE_OOO_MASK, 1},
    {4, 6, WHITE_OO_MASK, 0},
    {60, 58, BLACK_OOO_MASK, 3},
    {60, 62, BLACK_OO_MASK, 2},
};

Board::Board(Attacks *_attacks) : attacks(_attacks) {}

bool Board::make_move(Move move)
{
    move.castle_rights = castle_rights;
    if (!validate_move(move))
        return false;

    ugly_move(move);

    return true;
}

inline void Board::ugly_move(Move move)
{
    COLORS opponent_color = is_white_turn ? COLORS::BLACK : COLORS::WHITE;

    remove_piece(move.from, move.color, move.piece);

    switch (move.type)
    {
    case MOVE_TYPE::NORMAL:
    {
        add_piece(move.to, move.color, move.piece);
        break;
    }
    case MOVE_TYPE::CAPTURE:
    {
        remove_piece(move.to, opponent_color, move.captured);
        add_piece(move.to, move.color, move.piece);
        break;
    }
    case MOVE_TYPE::EN_PASSANT:
    {
        int captured_pawn_sq = move.to + (move.color == COLORS::WHITE ? -8 : 8);
        remove_piece(captured_pawn_sq, opponent_color, PIECE_TYPE::PAWN);
        add_piece(move.to, move.color, PIECE_TYPE::PAWN);
        break;
    }
    case MOVE_TYPE::PROMOTION:
    {
        if (move.captured != PIECE_TYPE::NONE)
            remove_piece(move.to, opponent_color, move.captured);
        add_piece(move.to, move.color, move.promoutioned);
        break;
    }
    case MOVE_TYPE::CASTLE:
    {
        add_piece(move.to, move.color, PIECE_TYPE::KING);

        int rook_from, rook_to;
        if (move.to > move.from)
        {
            rook_from = move.from + 3;
            rook_to = move.from + 1;
        }
        else
        {
            rook_from = move.from - 4;
            rook_to = move.from - 1;
        }

        remove_piece(rook_from, move.color, PIECE_TYPE::ROOK);
        add_piece(rook_to, move.color, PIECE_TYPE::ROOK);
        break;
    }
    }

    if (move.piece == PIECE_TYPE::KING)
    {
        if (move.color == COLORS::WHITE)
            castle_rights &= ~0b0011;
        else
            castle_rights &= ~0b1100;
    }
    else if (move.piece == PIECE_TYPE::ROOK)
    {
        if (move.from == 0)
            castle_rights &= ~0b0010;
        else if (move.from == 7)
            castle_rights &= ~0b0001;
        else if (move.from == 56)
            castle_rights &= ~0b1000;
        else if (move.from == 63)
            castle_rights &= ~0b0100;
    }

    if (move.type == MOVE_TYPE::CAPTURE || move.type == MOVE_TYPE::PROMOTION)
    {
        if (move.to == 0)
            castle_rights &= ~0b0010;
        else if (move.to == 7)
            castle_rights &= ~0b0001;
        else if (move.to == 56)
            castle_rights &= ~0b1000;
        else if (move.to == 63)
            castle_rights &= ~0b0100;
    }

    w_pieces = w_king | w_queen | w_rooks | w_bishops | w_knights | w_pawns;
    b_pieces = b_king | b_queen | b_rooks | b_bishops | b_knights | b_pawns;

    history.push_back(move);

    is_white_turn = !is_white_turn;
}

void Board::undo_move()
{
    if (history.empty())
        return;

    Move move = history.back();
    history.pop_back();

    castle_rights = move.castle_rights;

    is_white_turn = !is_white_turn;
    COLORS opponent_color = is_white_turn ? COLORS::BLACK : COLORS::WHITE;

    switch (move.type)
    {
    case MOVE_TYPE::NORMAL:
    {
        remove_piece(move.to, move.color, move.piece);
        add_piece(move.from, move.color, move.piece);
        break;
    }
    case MOVE_TYPE::CAPTURE:
    {
        remove_piece(move.to, move.color, move.piece);
        add_piece(move.from, move.color, move.piece);
        add_piece(move.to, opponent_color, move.captured);
        break;
    }
    case MOVE_TYPE::EN_PASSANT:
    {
        remove_piece(move.to, move.color, PIECE_TYPE::PAWN);
        add_piece(move.from, move.color, PIECE_TYPE::PAWN);
        int captured_pawn_sq = move.to + (move.color == COLORS::WHITE ? -8 : 8);
        add_piece(captured_pawn_sq, opponent_color, PIECE_TYPE::PAWN);
        break;
    }
    case MOVE_TYPE::PROMOTION:
    {
        remove_piece(move.to, move.color, move.promoutioned);
        add_piece(move.from, move.color, PIECE_TYPE::PAWN);
        if (move.captured != PIECE_TYPE::NONE)
            add_piece(move.to, opponent_color, move.captured);
        break;
    }
    case MOVE_TYPE::CASTLE:
    {
        remove_piece(move.to, move.color, PIECE_TYPE::KING);
        add_piece(move.from, move.color, PIECE_TYPE::KING);

        int rook_from, rook_to;
        if (move.to > move.from)
        {
            rook_from = move.from + 3;
            rook_to = move.from + 1;
        }
        else
        {
            rook_from = move.from - 4;
            rook_to = move.from - 1;
        }

        remove_piece(rook_to, move.color, PIECE_TYPE::ROOK);
        add_piece(rook_from, move.color, PIECE_TYPE::ROOK);
        break;
    }
    }

    w_pieces = w_king | w_queen | w_rooks | w_bishops | w_knights | w_pawns;
    b_pieces = b_king | b_queen | b_rooks | b_bishops | b_knights | b_pawns;
}

bool Board::validate_move(Move move)
{
    if ((move.color == COLORS::WHITE) != is_white_turn)
        return false;
    if (move.from == move.to)
        return false;

    bool is_valid = false;
    switch (move.piece)
    {
    case PIECE_TYPE::KING:
        is_valid = validate_king_move(move);
        break;

    case PIECE_TYPE::PAWN:
        is_valid = validate_pawn_move(move);
        break;

    case PIECE_TYPE::QUEEN:
    case PIECE_TYPE::ROOK:
    case PIECE_TYPE::BISHOP:
    case PIECE_TYPE::KNIGHT:
        is_valid = validate_basic_attack(move);
        break;

    default:
        return false;
    }

    if (!is_valid)
        return false;

    move.castle_rights = castle_rights;

    ugly_move(move);

    bitboard king_bb = (move.color == COLORS::WHITE) ? w_king : b_king;
    int king_position = bitScanForward(king_bb);

    bool res = !is_king_checked(move.color, king_position);
    undo_move();

    return res;
}

bool Board::validate_king_move(Move move)
{
    if (validate_basic_attack(move))
        return true;

    if (move.type == MOVE_TYPE::CASTLE)
    {
        int offset = move.color == COLORS::WHITE ? 0 : 2;
        bitboard blockers = w_pieces | b_pieces;

        for (int i = 0; i < 2; i++)
        {
            auto &[from, to, mask, rights_bit] = castles[offset + i];
            if (move.from == from && move.to == to)
            {   
                if (mask & blockers) {
                    return false;
                }
                if ((castle_rights & (1 << rights_bit)) == 0) {
                    return false;
                }

                bool is_valid = true;
                int step = from < to ? +1 : -1;
                for (int sq = from; sq != to + step; sq += step) {
                    bool checked = is_king_checked(move.color, sq);
                    if (checked) {
                        is_valid = false;
                        break;
                    }
                }
                
                return is_valid;
            }
        }
    }

    return false;
}

bool Board::validate_pawn_move(Move move)
{
    bool is_white = move.color == COLORS::WHITE;
    bitboard pawns = is_white ? w_pawns : b_pawns;
    bitboard blockers = w_pieces | b_pieces;

    if (((1ULL << move.from) & pawns) == 0)
        return false;

    switch (move.type)
    {
    case MOVE_TYPE::NORMAL:
    {
        if (move.from % 8 != move.to % 8)
            return false;
        bitboard moves = attacks->get_pawn_advances(move.color, pawns, blockers);
        return (1ULL << move.to) & moves & ~blockers;
    }
    case MOVE_TYPE::CAPTURE:
    {
        if (std::abs(move.from % 8 - move.to % 8) != 1)
            return false;
        bitboard moves = attacks->get_pawn_attacks(move.color, pawns);
        COLORS opponent_color = is_white ? COLORS::BLACK : COLORS::WHITE;
        bitboard captured = *get_bitboard_by_piece(opponent_color, move.captured);

        return (1ULL << move.to) & moves & captured;
        break;
    }
    case MOVE_TYPE::EN_PASSANT:
    {
        int history_size = history.size();
        if (history_size == 0)
            return false;

        Move last_move = history[history_size - 1];

        if (last_move.piece != PIECE_TYPE::PAWN)
            return false;

        int from_rank = is_white ? 6 : 1, to_rank = is_white ? 4 : 3;
        int last_move_from_rank = last_move.from / 8;
        int last_move_to_rank = last_move.to / 8;
        int move_from_rank = move.from / 8;

        if (last_move_from_rank != from_rank || last_move_to_rank != to_rank)
            return false;

        if (move_from_rank != last_move_to_rank)
            return false;

        if (std::abs(move.from % 8 - last_move.to % 8) != 1)
            return false;

        int step = is_white ? -8 : +8;
        if (move.to + step != last_move.to)
            return false;

        return true;
    }
    case MOVE_TYPE::PROMOTION:
    {
        if (move.promoutioned == PIECE_TYPE::KING || move.promoutioned == PIECE_TYPE::PAWN)
            return false;

        int expected_rank = is_white ? 6 : 1;
        if (move.from / 8 != expected_rank)
            return false;

        int target_rank = is_white ? 7 : 0;
        if (move.to / 8 != target_rank)
            return false;

        int diff = std::abs(move.from - move.to);
        bitboard target_bit = (1ULL << move.to);

        if (diff == 8)
            return (target_bit & ~blockers);

        else if (diff == 7 || diff == 9)
        {
            if (std::abs((move.from % 8) - (move.to % 8)) != 1)
                return false;

            bitboard enemy_blockers = is_white ? b_pieces : w_pieces;
            return (target_bit & enemy_blockers);
        }

        return false;
    }

    default:
        return false;
    }
}

bool Board::validate_basic_attack(Move move)
{
    bool is_white = move.color == COLORS::WHITE;

    bitboard piece = *get_bitboard_by_piece(move.color, move.piece);
    bitboard piece_attacks;
    bitboard blockers = w_pieces | b_pieces;

    switch (move.piece)
    {
    case PIECE_TYPE::KING:
        piece_attacks = attacks->get_king_attacks(piece);
        break;
    case PIECE_TYPE::QUEEN:
        piece_attacks = attacks->get_queen_attacks(move.from, blockers);
        break;
    case PIECE_TYPE::ROOK:
        piece_attacks = attacks->get_rook_attacks(move.from, blockers);
        break;
    case PIECE_TYPE::BISHOP:
        piece_attacks = attacks->get_bishop_attacks(move.from, blockers);
        break;
    case PIECE_TYPE::KNIGHT:
        piece_attacks = attacks->get_knights_attacks(piece);
        break;

    default:
        return false;
    }

    if (((1ULL << move.from) & piece) == 0)
        return false;

    switch (move.type)
    {
    case MOVE_TYPE::NORMAL:
        return (1ULL << move.to) & piece_attacks & ~blockers;

    case MOVE_TYPE::CAPTURE:
    {

        COLORS opponent_color = is_white ? COLORS::BLACK : COLORS::WHITE;
        bitboard opponent_piece = *get_bitboard_by_piece(opponent_color, move.captured);
        return (1ULL << move.to) & piece_attacks & opponent_piece;
    }
    default:
        return false;
    }
}

void Board::update_piece(int sq, COLORS color, PIECE_TYPE piece, bool add)
{
    bitboard *target = get_bitboard_by_piece(color, piece);
    if (target)
    {
        if (add)
            *target |= (1ULL << sq);
        else
            *target &= ~(1ULL << sq);
    }
}

bitboard *Board::get_bitboard_by_piece(COLORS color, PIECE_TYPE piece)
{
    bitboard *target = nullptr;
    bool is_white = (color == COLORS::WHITE);

    switch (piece)
    {
    case PIECE_TYPE::KING:
        target = is_white ? &w_king : &b_king;
        break;
    case PIECE_TYPE::QUEEN:
        target = is_white ? &w_queen : &b_queen;
        break;
    case PIECE_TYPE::ROOK:
        target = is_white ? &w_rooks : &b_rooks;
        break;
    case PIECE_TYPE::BISHOP:
        target = is_white ? &w_bishops : &b_bishops;
        break;
    case PIECE_TYPE::KNIGHT:
        target = is_white ? &w_knights : &b_knights;
        break;
    case PIECE_TYPE::PAWN:
        target = is_white ? &w_pawns : &b_pawns;
        break;
    default:
        return 0;
    }

    return target;
}

void Board::add_piece(int sq, COLORS color, PIECE_TYPE piece)
{
    update_piece(sq, color, piece, true);
}

void Board::remove_piece(int sq, COLORS color, PIECE_TYPE piece)
{
    update_piece(sq, color, piece, false);
}

bool Board::is_king_checked(COLORS color, int sq)
{
    bool is_white = color == COLORS::WHITE;
    bitboard blockers = w_pieces | b_pieces;

    bitboard L = attacks->get_knights_attacks(1ULL << sq);
    if ((is_white ? b_knights : w_knights) & L)
        return true;

    bitboard diagonal = attacks->get_bishop_attacks(sq, blockers);
    if ((is_white ? b_bishops | b_queen : w_bishops | w_queen) & diagonal)
        return true;

    bitboard straight = attacks->get_rook_attacks(sq, blockers);
    if ((is_white ? b_rooks | b_queen : w_rooks | w_queen) & straight)
        return true;

    bitboard pawns = attacks->get_pawn_attacks(color, 1ULL << sq);
    if ((is_white ? b_pawns : w_pawns) & pawns)
        return true;

    bitboard king = attacks->get_king_attacks(1ULL << sq);
    if ((is_white ? b_king : w_king) & king)
        return true;

    return false;
}

void Board::set_default()
{
    b_king = BLACK_KING;
    b_queen = BLACK_QUEEN;
    b_rooks = BLACK_ROOKS;
    b_bishops = BLACK_BISHOPS;
    b_knights = BLACK_KNIGHTS;
    b_pawns = RANK_7;

    w_king = WHITE_KING;
    w_queen = WHITE_QUEEN;
    w_rooks = WHITE_ROOKS;
    w_bishops = WHITE_BISHOPS;
    w_knights = WHITE_KNIGHTS;
    w_pawns = RANK_2;

    w_pieces = w_king | w_queen | w_rooks | w_bishops | w_knights | w_pawns;
    b_pieces = b_king | b_queen | b_rooks | b_bishops | b_knights | b_pawns;
}
