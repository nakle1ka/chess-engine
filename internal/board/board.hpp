#pragma once

#include <cstdint>
#include <vector>
#include "../attacks/attacks.hpp"

class Board
{
private:
    Attacks *attacks;
    std::vector<Move> history;
    bool is_white_turn = true;
    uint8_t castle_rights = 0b1111; // use the first 4 bits

    bitboard b_king = 0, b_queen = 0, b_rooks = 0, b_bishops = 0, b_knights = 0, b_pawns = 0;
    bitboard w_king = 0, w_queen = 0, w_rooks = 0, w_bishops = 0, w_knights = 0, w_pawns = 0;

    bitboard w_pieces, b_pieces;

public:
    Board(Attacks *_attacks);

    void set_default();

    bool make_move(Move move);
    void ugly_move(Move move);
    void undo_move();

    void add_piece(int sq, COLORS color, PIECE_TYPE piece);
    void remove_piece(int sq, COLORS color, PIECE_TYPE piece);

    bitboard get_position()
    {
        return w_pieces | b_pieces;
    }

    bitboard getBlackKing() const { return b_king; }
    bitboard getBlackQueen() const { return b_queen; }
    bitboard getBlackRooks() const { return b_rooks; }
    bitboard getBlackBishops() const { return b_bishops; }
    bitboard getBlackKnights() const { return b_knights; }
    bitboard getBlackPawns() const { return b_pawns; }

  
    bitboard getWhiteKing() const { return w_king; }
    bitboard getWhiteQueen() const { return w_queen; }
    bitboard getWhiteRooks() const { return w_rooks; }
    bitboard getWhiteBishops() const { return w_bishops; }
    bitboard getWhiteKnights() const { return w_knights; }
    bitboard getWhitePawns() const { return w_pawns; }

    COLORS get_turn_color()
    {
        return is_white_turn ? COLORS::WHITE : COLORS::BLACK;
    }

private:
    bool validate_move(Move move);
    bool validate_king_move(Move move);
    bool validate_pawn_move(Move move);
    bool validate_basic_attack(Move move);

    bool is_king_checked(COLORS color, int sq);
    void update_piece(int sq, COLORS color, PIECE_TYPE piece, bool add);
    bitboard *get_bitboard_by_piece(COLORS color, PIECE_TYPE piece);
};