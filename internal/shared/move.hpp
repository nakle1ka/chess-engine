#pragma once
#include <cstdint>
#include "../shared/utils.hpp"

struct Move
{
    uint64_t data;

    Move() {};

    Move(
        int from, int to,
        MOVE_TYPE type,
        COLORS color,
        PIECE_TYPE piece,
        PIECE_TYPE captured,
        PIECE_TYPE promotiuned,
        uint8_t castle_rights,
        uint8_t en_passant_square)
    {
        data = 0;
        data |= (uint64_t(from) & 0x3F) << 0;
        data |= (uint64_t(to) & 0x3F) << 6;
        data |= (uint64_t(color) & 0x01) << 12;
        data |= (uint64_t(piece) & 0x07) << 13;
        data |= (uint64_t(captured) & 0x07) << 16;
        data |= (uint64_t(promotiuned) & 0x07) << 19;
        data |= (uint64_t(castle_rights) & 0x0F) << 22;
        data |= (uint64_t(type) & 0x07) << 26;
        data |= (uint64_t(en_passant_square) & 0x7F) << 29;
    }

    inline void set_from(int from)
    {
        data &= ~0x3FULL;
        data |= (uint64_t(from) & 0x3F);
    }

    inline void set_to(int to)
    {
        data &= ~(0x3FULL << 6);
        data |= (uint64_t(to) & 0x3F) << 6;
    }

    inline void set_color(COLORS color)
    {
        data &= ~(1ULL << 12);
        data |= (uint64_t(color) & 1) << 12;
    }

    inline void set_piece(PIECE_TYPE piece)
    {
        data &= ~(0x7ULL << 13);
        data |= (uint64_t(piece) & 0x7) << 13;
    }

    inline void set_captured(PIECE_TYPE captured)
    {
        data &= ~(0x7ULL << 16);
        data |= (uint64_t(captured) & 0x7) << 16;
    }

    inline void set_promotion(PIECE_TYPE promotion)
    {
        data &= ~(0x7ULL << 19);
        data |= (uint64_t(promotion) & 0x7) << 19;
    }

    inline void set_castle_rights(uint8_t castle_rights)
    {
        data &= ~(0xFULL << 22);
        data |= (uint64_t(castle_rights) & 0xF) << 22;
    }

    inline void set_type(MOVE_TYPE type)
    {
        data &= ~(0x7ULL << 26);
        data |= (uint64_t(type) & 0x7) << 26;
    }

    inline void set_en_passant_square(uint8_t en_passant_square)
    {
        data &= ~(0x7FULL << 29);
        data |= (uint64_t(en_passant_square) & 0x7F) << 29;
    }

    inline int get_from()
    {
        return get_bites(data, 0, 5);
    }

    inline int get_to()
    {
        return get_bites(data, 6, 11);
    }

    inline COLORS get_color()
    {
        return static_cast<COLORS>(get_bites(data, 12, 12));
    }

    inline PIECE_TYPE get_piece()
    {
        return static_cast<PIECE_TYPE>(get_bites(data, 13, 15));
    }

    inline PIECE_TYPE get_captured()
    {
        return static_cast<PIECE_TYPE>(get_bites(data, 16, 18));
    }

    inline PIECE_TYPE get_promoutioned()
    {
        return static_cast<PIECE_TYPE>(get_bites(data, 19, 21));
    }

    inline uint8_t get_castle_rights()
    {
        return static_cast<uint8_t>(get_bites(data, 22, 25));
    }

    inline MOVE_TYPE get_type()
    {
        return static_cast<MOVE_TYPE>(get_bites(data, 26, 28));
    }

    inline uint8_t get_en_passant_square()
    {
        return static_cast<uint8_t>(get_bites(data, 29, 35));
    }
};