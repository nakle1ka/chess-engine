#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include "../attacks/attacks.hpp"
#include "../shared/move.hpp"

struct MoveList
{
    Move moves[256];
    int count = 0;

    inline void add(Move move)
    {
        moves[count++] = move;
    }
};

class Board
{
private:
    Attacks *attacks;

    std::vector<Move> history;
    bool is_white_turn = true;
    uint8_t castle_rights = 0b0000;

    std::pair<PIECE_TYPE, COLORS> mailbox[64];

    bitboard w_pieces = 0, b_pieces = 0;
    bitboard b_king = 0, b_queen = 0, b_rooks = 0, b_bishops = 0, b_knights = 0, b_pawns = 0;
    bitboard w_king = 0, w_queen = 0, w_rooks = 0, w_bishops = 0, w_knights = 0, w_pawns = 0;

public:
    Board(Attacks *_attacks);

    void set_default();
    std::pair<PIECE_TYPE, COLORS> piece_at(int sq);

    bool is_game_end();

    bool make_move(Move move);
    void ugly_move(Move move);
    void undo_move();

    void add_piece(int sq, COLORS color, PIECE_TYPE piece);
    void remove_piece(int sq, COLORS color, PIECE_TYPE piece);

    MoveList ugly_moves();

    bool is_king_checked(COLORS color);

    COLORS get_turn_color() const;
    

private:
    bool validate_move(Move move);
    bool validate_king_move(Move move);
    bool validate_pawn_move(Move move);
    bool validate_basic_attack(Move move);

    void add_piece_moves(int from, COLORS color, bitboard attack_mask, MoveList &list);
    void add_pawns_moves(COLORS color, MOVE_TYPE type, bitboard attack_mask, int shift, MoveList &list);
    void add_pawns_promotion_moves(COLORS color, bitboard attack_mask, int shift, MoveList &list);

    bool is_square_attacked(COLORS color, int sq);
    void update_piece(int sq, COLORS color, PIECE_TYPE piece, bool add);
    bitboard *get_bitboard_by_piece(COLORS color, PIECE_TYPE piece);
};