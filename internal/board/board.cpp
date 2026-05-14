#include <tuple>
#include "board.hpp"
#include "../shared/types.hpp"
#include "../shared/constants.hpp"
#include "../shared/utils.hpp"

// from-to position, mask, rights index
std::tuple<int, int, bitboard, int> castles[] = {
    {4, 6, WHITE_OO_MASK, 0},
    {4, 2, WHITE_OOO_MASK, 1},
    {60, 62, BLACK_OO_MASK, 2},
    {60, 58, BLACK_OOO_MASK, 3},
};

Board::Board(Attacks *_attacks) : attacks(_attacks)
{
    for (auto &sq : mailbox)
        sq = {PIECE_TYPE::NONE, COLORS::WHITE};
}

bool Board::is_game_end()
{
    MoveList moves = ugly_moves();
    COLORS color = get_turn_color();

    for (int i = 0; i < moves.count; i++)
    {
        ugly_move(moves.moves[i]);

        if (!is_king_checked(color))
        {
            undo_move();
            return false;
        }

        undo_move();
    }

    return true;
}

MoveList Board::ugly_moves()
{
    MoveList list;
    COLORS color = is_white_turn ? COLORS::WHITE : COLORS::BLACK;

    bitboard king = is_white_turn ? w_king : b_king;
    bitboard queen = is_white_turn ? w_queen : b_queen;
    bitboard rooks = is_white_turn ? w_rooks : b_rooks;
    bitboard bishops = is_white_turn ? w_bishops : b_bishops;
    bitboard knights = is_white_turn ? w_knights : b_knights;
    bitboard pawns = is_white_turn ? w_pawns : b_pawns;

    bitboard blockers = w_pieces | b_pieces;
    bitboard my_pieces = is_white_turn ? w_pieces : b_pieces;
    bitboard opponent_pieces = is_white_turn ? b_pieces : w_pieces;
    bitboard rank_8_or_1 = is_white_turn ? RANK_8 : RANK_1;

    bitboard short_push = is_white_turn
                              ? (pawns << 8) & ~blockers
                              : (pawns >> 8) & ~blockers;

    bitboard normal_short = short_push & ~rank_8_or_1;
    bitboard promo_short = short_push & rank_8_or_1;

    bitboard start_rank = is_white_turn ? RANK_3 : RANK_6;
    bitboard long_push = is_white_turn
                             ? ((short_push & start_rank) << 8) & ~blockers
                             : ((short_push & start_rank) >> 8) & ~blockers;

    bitboard left_attack = is_white_turn
                               ? ((pawns << 7) & ~FILE_H) & opponent_pieces
                               : ((pawns >> 9) & ~FILE_H) & opponent_pieces;

    bitboard right_attack = is_white_turn
                                ? ((pawns << 9) & ~FILE_A) & opponent_pieces
                                : ((pawns >> 7) & ~FILE_A) & opponent_pieces;

    bitboard normal_left = left_attack & ~rank_8_or_1;
    bitboard promo_left = left_attack & rank_8_or_1;

    bitboard normal_right = right_attack & ~rank_8_or_1;
    bitboard promo_right = right_attack & rank_8_or_1;

    add_pawns_promotion_moves(color, promo_short, 8, list);
    add_pawns_promotion_moves(color, promo_left, is_white_turn ? 7 : 9, list);
    add_pawns_promotion_moves(color, promo_right, is_white_turn ? 9 : 7, list);
    add_pawns_moves(color, MOVE_TYPE::CAPTURE, normal_left, is_white_turn ? 7 : 9, list);
    add_pawns_moves(color, MOVE_TYPE::CAPTURE, normal_right, is_white_turn ? 9 : 7, list);
    add_pawns_moves(color, MOVE_TYPE::NORMAL, normal_short, 8, list);
    add_pawns_moves(color, MOVE_TYPE::NORMAL, long_push, 16, list);

    while (knights)
    {
        int from = bitScanForward(knights);
        knights &= knights - 1;
        bitboard knight_mask = attacks->get_knights_attacks(1ULL << from) & ~my_pieces;
        add_piece_moves(from, color, knight_mask, list);
    }

    while (king)
    {
        int from = bitScanForward(king);
        king &= king - 1;
        bitboard king_mask = attacks->get_king_attacks(1ULL << from) & ~my_pieces;
        add_piece_moves(from, color, king_mask, list);
    }

    while (bishops)
    {
        int from = bitScanForward(bishops);
        bishops &= bishops - 1;
        bitboard bishop_mask = attacks->get_bishop_attacks(from, blockers) & ~my_pieces;
        add_piece_moves(from, color, bishop_mask, list);
    }

    while (rooks)
    {
        int from = bitScanForward(rooks);
        rooks &= rooks - 1;
        bitboard rook_mask = attacks->get_rook_attacks(from, blockers) & ~my_pieces;
        add_piece_moves(from, color, rook_mask, list);
    }

    while (queen)
    {
        int from = bitScanForward(queen);
        queen &= queen - 1;
        bitboard queen_mask = attacks->get_queen_attacks(from, blockers) & ~my_pieces;
        add_piece_moves(from, color, queen_mask, list);
    }

    if (is_white_turn && (castle_rights & (1ULL << 0)) && (blockers & WHITE_OO_MASK) == 0)
        list.add({4, 6,
                  MOVE_TYPE::CASTLE,
                  COLORS::WHITE,
                  PIECE_TYPE::KING,
                  PIECE_TYPE::NONE,
                  PIECE_TYPE::NONE,
                  castle_rights});

    if (is_white_turn && (castle_rights & (1ULL << 1)) && (blockers & WHITE_OOO_MASK) == 0)
        list.add({4, 2,
                  MOVE_TYPE::CASTLE,
                  COLORS::WHITE,
                  PIECE_TYPE::KING,
                  PIECE_TYPE::NONE,
                  PIECE_TYPE::NONE,
                  castle_rights});

    if (!is_white_turn && (castle_rights & (1ULL << 2)) && (blockers & BLACK_OO_MASK) == 0)
        list.add({60, 62,
                  MOVE_TYPE::CASTLE,
                  COLORS::BLACK,
                  PIECE_TYPE::KING,
                  PIECE_TYPE::NONE,
                  PIECE_TYPE::NONE,
                  castle_rights});

    if (!is_white_turn && (castle_rights & (1ULL << 3)) && (blockers & BLACK_OOO_MASK) == 0)
        list.add({60, 58,
                  MOVE_TYPE::CASTLE,
                  COLORS::BLACK,
                  PIECE_TYPE::KING,
                  PIECE_TYPE::NONE,
                  PIECE_TYPE::NONE,
                  castle_rights});

    return list;
}

bool Board::is_king_checked(COLORS color)
{
    bitboard king = color == COLORS::WHITE ? w_king : b_king;
    int king_position = bitScanForward(king);
    return is_square_attacked(reverse_color(color), king_position);
}

void Board::add_piece_moves(
    int from, COLORS color,
    bitboard attack_mask,
    MoveList &list)
{
    PIECE_TYPE piece = mailbox[from].first;
    while (attack_mask)
    {
        int to = bitScanForward(attack_mask);
        PIECE_TYPE opponent_piece = mailbox[to].first;
        attack_mask &= attack_mask - 1;

        Move move(
            from, to,
            opponent_piece == PIECE_TYPE::NONE
                ? MOVE_TYPE::NORMAL
                : MOVE_TYPE::CAPTURE,
            color,
            piece, opponent_piece,
            PIECE_TYPE::NONE,
            castle_rights);

        list.add(move);
    }
}

void Board::add_pawns_moves(
    COLORS color, MOVE_TYPE type,
    bitboard attack_mask, int shift,
    MoveList &list)
{
    shift = (color == COLORS::WHITE) ? shift : -shift;

    while (attack_mask)
    {
        int to = bitScanForward(attack_mask);
        int from = to - shift;
        attack_mask &= attack_mask - 1;

        Move move(
            from, to,
            type, color,
            PIECE_TYPE::PAWN,
            mailbox[to].first,
            PIECE_TYPE::NONE,
            castle_rights);
        list.add(move);
    }
}

void Board::add_pawns_promotion_moves(COLORS color, bitboard attack_mask, int shift, MoveList &list)
{
    while (attack_mask)
    {
        int to = bitScanForward(attack_mask);
        int from = to - shift;
        attack_mask &= attack_mask - 1;

        Move move(
            from, to,
            MOVE_TYPE::PROMOTION,
            color,
            PIECE_TYPE::PAWN,
            mailbox[to].first,
            PIECE_TYPE::QUEEN,
            castle_rights);
        list.add(move);

        move.set_promotion(PIECE_TYPE::ROOK);
        list.add(move);

        move.set_promotion(PIECE_TYPE::BISHOP);
        list.add(move);

        move.set_promotion(PIECE_TYPE::KNIGHT);
        list.add(move);
    }
}

bool Board::make_move(Move move)
{
    if (!validate_move(move))
        return false;

    ugly_move(move);

    return true;
}

void Board::ugly_move(Move move)
{
    move.set_castle_rights(castle_rights);
    COLORS opponent_color = reverse_color(move.get_color());

    remove_piece(move.get_from(), move.get_color(), move.get_piece());

    switch (move.get_type())
    {
    case MOVE_TYPE::NORMAL:
    {
        add_piece(move.get_to(), move.get_color(), move.get_piece());
        break;
    }
    case MOVE_TYPE::CAPTURE:
    {
        remove_piece(move.get_to(), opponent_color, move.get_captured());
        add_piece(move.get_to(), move.get_color(), move.get_piece());
        break;
    }
    case MOVE_TYPE::EN_PASSANT:
    {
        int captured_pawn_sq = move.get_to() + (move.get_color() == COLORS::WHITE ? -8 : 8);
        remove_piece(captured_pawn_sq, opponent_color, PIECE_TYPE::PAWN);
        add_piece(move.get_to(), move.get_color(), PIECE_TYPE::PAWN);
        break;
    }
    case MOVE_TYPE::PROMOTION:
    {
        if (move.get_captured() != PIECE_TYPE::NONE)
            remove_piece(move.get_to(), opponent_color, move.get_captured());
        add_piece(move.get_to(), move.get_color(), move.get_promoutioned());
        break;
    }
    case MOVE_TYPE::CASTLE:
    {
        add_piece(move.get_to(), move.get_color(), PIECE_TYPE::KING);

        int rook_from, rook_to;
        if (move.get_to() > move.get_from())
        {
            rook_from = move.get_from() + 3;
            rook_to = move.get_from() + 1;
        }
        else
        {
            rook_from = move.get_from() - 4;
            rook_to = move.get_from() - 1;
        }

        remove_piece(rook_from, move.get_color(), PIECE_TYPE::ROOK);
        add_piece(rook_to, move.get_color(), PIECE_TYPE::ROOK);
        break;
    }
    }

    if (move.get_piece() == PIECE_TYPE::KING)
    {
        if (move.get_color() == COLORS::WHITE)
            castle_rights &= ~0b0011;
        else
            castle_rights &= ~0b1100;
    }
    else if (move.get_piece() == PIECE_TYPE::ROOK)
    {
        if (move.get_from() == 0)
            castle_rights &= ~0b0010;
        else if (move.get_from() == 7)
            castle_rights &= ~0b0001;
        else if (move.get_from() == 56)
            castle_rights &= ~0b1000;
        else if (move.get_from() == 63)
            castle_rights &= ~0b0100;
    }

    if (move.get_type() == MOVE_TYPE::CAPTURE || move.get_type() == MOVE_TYPE::PROMOTION)
    {
        if (move.get_to() == 0)
            castle_rights &= ~0b0010;
        else if (move.get_to() == 7)
            castle_rights &= ~0b0001;
        else if (move.get_to() == 56)
            castle_rights &= ~0b1000;
        else if (move.get_to() == 63)
            castle_rights &= ~0b0100;
    }

    history.push_back(move);

    is_white_turn = !is_white_turn;
}

void Board::undo_move()
{
    if (history.empty())
        return;

    Move move = history.back();
    history.pop_back();

    is_white_turn = !is_white_turn;
    COLORS opponent_color = is_white_turn ? COLORS::BLACK : COLORS::WHITE;

    switch (move.get_type())
    {
    case MOVE_TYPE::NORMAL:
    {
        remove_piece(move.get_to(), move.get_color(), move.get_piece());
        add_piece(move.get_from(), move.get_color(), move.get_piece());
        break;
    }
    case MOVE_TYPE::CAPTURE:
    {
        remove_piece(move.get_to(), move.get_color(), move.get_piece());
        add_piece(move.get_from(), move.get_color(), move.get_piece());
        add_piece(move.get_to(), opponent_color, move.get_captured());
        break;
    }
    case MOVE_TYPE::EN_PASSANT:
    {
        remove_piece(move.get_to(), move.get_color(), PIECE_TYPE::PAWN);
        add_piece(move.get_from(), move.get_color(), PIECE_TYPE::PAWN);
        int captured_pawn_sq = move.get_to() + (move.get_color() == COLORS::WHITE ? -8 : 8);
        add_piece(captured_pawn_sq, opponent_color, PIECE_TYPE::PAWN);
        break;
    }
    case MOVE_TYPE::PROMOTION:
    {
        remove_piece(move.get_to(), move.get_color(), move.get_promoutioned());
        add_piece(move.get_from(), move.get_color(), PIECE_TYPE::PAWN);
        if (move.get_captured() != PIECE_TYPE::NONE)
            add_piece(move.get_to(), opponent_color, move.get_captured());
        break;
    }
    case MOVE_TYPE::CASTLE:
    {
        remove_piece(move.get_to(), move.get_color(), PIECE_TYPE::KING);
        add_piece(move.get_from(), move.get_color(), PIECE_TYPE::KING);

        int rook_from, rook_to;
        if (move.get_to() > move.get_from())
        {
            rook_from = move.get_from() + 3;
            rook_to = move.get_from() + 1;
        }
        else
        {
            rook_from = move.get_from() - 4;
            rook_to = move.get_from() - 1;
        }

        remove_piece(rook_to, move.get_color(), PIECE_TYPE::ROOK);
        add_piece(rook_from, move.get_color(), PIECE_TYPE::ROOK);
        break;
    }
    }

    castle_rights = move.get_castle_rights();
}

bool Board::validate_move(Move move)
{
    if ((move.get_color() == COLORS::WHITE) != is_white_turn)
        return false;
    if (move.get_from() == move.get_to())
        return false;

    bool is_valid = false;
    switch (move.get_piece())
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

    ugly_move(move);

    bitboard king_bb = (move.get_color() == COLORS::WHITE) ? w_king : b_king;
    int king_position = bitScanForward(king_bb);

    bool res = !is_square_attacked(reverse_color(move.get_color()), king_position);
    undo_move();

    return res;
}

bool Board::validate_king_move(Move move)
{
    if (validate_basic_attack(move))
        return true;

    if (move.get_type() == MOVE_TYPE::CASTLE)
    {
        int offset = move.get_color() == COLORS::WHITE ? 0 : 2;
        bitboard blockers = w_pieces | b_pieces;

        COLORS opponent_color = reverse_color(move.get_color());

        for (int i = 0; i < 2; i++)
        {
            auto &[from, to, mask, rights_bit] = castles[offset + i];
            if (move.get_from() == from && move.get_to() == to)
            {
                if (mask & blockers)
                    return false;
                if ((castle_rights & (1 << rights_bit)) == 0)
                    return false;

                return !is_line_attacked(opponent_color, from, to);
            }
        }
    }

    return false;
}

bool Board::validate_pawn_move(Move move)
{
    bool is_white = move.get_color() == COLORS::WHITE;
    bitboard pawns = is_white ? w_pawns : b_pawns;
    bitboard blockers = w_pieces | b_pieces;

    if (((1ULL << move.get_from()) & pawns) == 0)
        return false;

    switch (move.get_type())
    {
    case MOVE_TYPE::NORMAL:
    {
        if (move.get_from() % 8 != move.get_to() % 8)
            return false;
        bitboard moves = attacks->get_pawn_advances(move.get_color(), pawns, blockers);
        return (1ULL << move.get_to()) & moves & ~blockers;
    }
    case MOVE_TYPE::CAPTURE:
    {
        if (std::abs(move.get_from() % 8 - move.get_to() % 8) != 1)
            return false;
        bitboard moves = attacks->get_pawn_attacks(move.get_color(), pawns);
        COLORS opponent_color = is_white ? COLORS::BLACK : COLORS::WHITE;
        bitboard captured = *get_bitboard_by_piece(opponent_color, move.get_captured());

        return (1ULL << move.get_to()) & moves & captured;
        break;
    }
    case MOVE_TYPE::EN_PASSANT:
    {
        int history_size = history.size();
        if (history_size == 0)
            return false;

        Move last_move = history[history_size - 1];

        if (last_move.get_piece() != PIECE_TYPE::PAWN)
            return false;

        int from_rank = is_white ? 6 : 1, to_rank = is_white ? 4 : 3;
        int last_move_from_rank = last_move.get_from() / 8;
        int last_move_to_rank = last_move.get_to() / 8;
        int move_from_rank = move.get_from() / 8;

        if (last_move_from_rank != from_rank || last_move_to_rank != to_rank)
            return false;

        if (move_from_rank != last_move_to_rank)
            return false;

        if (std::abs(move.get_from() % 8 - last_move.get_to() % 8) != 1)
            return false;

        int step = is_white ? -8 : +8;
        if (move.get_to() + step != last_move.get_to())
            return false;

        return true;
    }
    case MOVE_TYPE::PROMOTION:
    {
        if (move.get_promoutioned() == PIECE_TYPE::KING || move.get_promoutioned() == PIECE_TYPE::PAWN)
            return false;

        int expected_rank = is_white ? 6 : 1;
        if (move.get_from() / 8 != expected_rank)
            return false;

        int target_rank = is_white ? 7 : 0;
        if (move.get_to() / 8 != target_rank)
            return false;

        int diff = std::abs(move.get_from() - move.get_to());
        bitboard target_bit = (1ULL << move.get_to());

        if (diff == 8)
            return (target_bit & ~blockers);

        else if (diff == 7 || diff == 9)
        {
            if (std::abs((move.get_from() % 8) - (move.get_to() % 8)) != 1)
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
    bitboard piece = *get_bitboard_by_piece(move.get_color(), move.get_piece());
    bitboard piece_attacks;
    bitboard blockers = w_pieces | b_pieces;

    switch (move.get_piece())
    {
    case PIECE_TYPE::KING:
        piece_attacks = attacks->get_king_attacks(piece);
        break;
    case PIECE_TYPE::QUEEN:
        piece_attacks = attacks->get_queen_attacks(move.get_from(), blockers);
        break;
    case PIECE_TYPE::ROOK:
        piece_attacks = attacks->get_rook_attacks(move.get_from(), blockers);
        break;
    case PIECE_TYPE::BISHOP:
        piece_attacks = attacks->get_bishop_attacks(move.get_from(), blockers);
        break;
    case PIECE_TYPE::KNIGHT:
        piece_attacks = attacks->get_knights_attacks(piece);
        break;

    default:
        return false;
    }

    if (((1ULL << move.get_from()) & piece) == 0)
        return false;

    switch (move.get_type())
    {
    case MOVE_TYPE::NORMAL:
        return (1ULL << move.get_to()) & piece_attacks & ~blockers;

    case MOVE_TYPE::CAPTURE:
    {
        COLORS opponent_color = reverse_color(move.get_color());
        bitboard opponent_piece = *get_bitboard_by_piece(opponent_color, move.get_captured());
        return (1ULL << move.get_to()) & piece_attacks & opponent_piece;
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
        bitboard mask = 1ULL << sq;
        if (add)
        {
            *target |= mask;
            mailbox[sq] = {piece, color};
            color == COLORS::WHITE ? w_pieces |= mask : b_pieces |= mask;
        }
        else
        {
            *target &= ~mask;
            mailbox[sq] = {PIECE_TYPE::NONE, COLORS::WHITE};
            color == COLORS::WHITE ? w_pieces &= ~mask : b_pieces &= ~mask;
        }
    }
}

void Board::add_piece(int sq, COLORS color, PIECE_TYPE piece)
{
    update_piece(sq, color, piece, true);
}

void Board::remove_piece(int sq, COLORS color, PIECE_TYPE piece)
{
    update_piece(sq, color, piece, false);
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

bool Board::is_square_attacked(COLORS color, int sq)
{
    bool is_white = color == COLORS::WHITE;
    bitboard blockers = b_pieces | w_pieces;

    bitboard L = attacks->get_knights_attacks(1ULL << sq);
    if ((is_white ? w_knights : b_knights) & L)
        return true;

    bitboard diagonal = attacks->get_bishop_attacks(sq, blockers);
    if ((is_white ? w_bishops | w_queen : b_bishops | b_queen) & diagonal)
        return true;

    bitboard straight = attacks->get_rook_attacks(sq, blockers);
    if ((is_white ? w_rooks | w_queen : b_rooks | b_queen) & straight)
        return true;

    bitboard pawns = attacks->get_pawn_attacks(reverse_color(color), 1ULL << sq);
    if ((is_white ? w_pawns : b_pawns) & pawns)
        return true;

    bitboard king = attacks->get_king_attacks(1ULL << sq);
    if ((is_white ? w_king : b_king) & king)
        return true;

    return false;
}

bool Board::is_line_attacked(COLORS color, int from, int to)
{
    if (from < to)
        std::swap(from, to);

    for (int i = from; i <= to; ++i)
        if (is_square_attacked(color, i))
            return true;

    return false;
}

void Board::set_default()
{
    castle_rights = 0b1111;

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

    mailbox[4] = {PIECE_TYPE::KING, COLORS::WHITE};
    mailbox[3] = {PIECE_TYPE::QUEEN, COLORS::WHITE};
    mailbox[0] = mailbox[7] = {PIECE_TYPE::ROOK, COLORS::WHITE};
    mailbox[5] = mailbox[2] = {PIECE_TYPE::BISHOP, COLORS::WHITE};
    mailbox[1] = mailbox[6] = {PIECE_TYPE::KNIGHT, COLORS::WHITE};

    mailbox[60] = {PIECE_TYPE::KING, COLORS::BLACK};
    mailbox[59] = {PIECE_TYPE::QUEEN, COLORS::BLACK};
    mailbox[63] = mailbox[56] = {PIECE_TYPE::ROOK, COLORS::BLACK};
    mailbox[61] = mailbox[58] = {PIECE_TYPE::BISHOP, COLORS::BLACK};
    mailbox[62] = mailbox[57] = {PIECE_TYPE::KNIGHT, COLORS::BLACK};

    for (int i = 0; i < 8; i++)
    {
        mailbox[8 + i] = {PIECE_TYPE::PAWN, COLORS::WHITE};
        mailbox[48 + i] = {PIECE_TYPE::PAWN, COLORS::BLACK};
    }
}

std::pair<PIECE_TYPE, COLORS> Board::piece_at(int sq)
{
    return mailbox[sq];
}

COLORS Board::get_turn_color() const
{
    return is_white_turn ? COLORS::WHITE : COLORS::BLACK;
}