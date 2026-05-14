#pragma once

#include <cstdint>

using bitboard = uint64_t;

enum class COLORS : uint8_t
{
    WHITE,
    BLACK
};

inline COLORS reverse_color(COLORS color) {
    return color == COLORS::WHITE ? COLORS::BLACK : COLORS::WHITE;
}

enum class MOVE_TYPE : uint8_t
{
    NORMAL,
    CAPTURE,
    CASTLE,
    EN_PASSANT,
    PROMOTION
};

enum class PIECE_TYPE : uint8_t
{
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN,
    NONE
};

enum class DIRECTION
{
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST,
    NORTH_WEST
};