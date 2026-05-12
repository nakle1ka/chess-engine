#pragma once

#include <cstdint>

using bitboard = uint64_t;

enum class COLORS
{
    WHITE,
    BLACK
};

enum class MOVE_TYPE
{
    NORMAL, 
    CAPTURE, 
    CASTLE, 
    EN_PASSANT, 
    PROMOTION
};

enum class PIECE_TYPE
{
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN,
    NONE
};

struct Move
{
    int from, to;
    MOVE_TYPE type;

    COLORS color;
    PIECE_TYPE piece;
    PIECE_TYPE captured;
    PIECE_TYPE promoutioned;

    uint8_t castle_rights;
};


enum class DIRECTION {
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST,
    NORTH_WEST
};