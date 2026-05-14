#pragma once
#include <cstdint>
#include "../shared/utils.hpp"

struct Move
{
    uint32_t data;

    Move() {};

    Move(
        uint32_t from, uint32_t to,
        MOVE_TYPE type,
        COLORS color,
        PIECE_TYPE piece,
        PIECE_TYPE captured,
        PIECE_TYPE promotiuned,
        uint8_t castle_rights)
    {
        data = 0;
        data |= from;
        data |= to << 6;
        data |= uint32_t(color) << 12;
        data |= uint32_t(piece) << 13;
        data |= uint32_t(captured) << 16;
        data |= uint32_t(promotiuned) << 19;
        data |= uint32_t(castle_rights) << 22;
        data |= uint32_t(type) << 26;
    }

    inline void set_from(uint32_t from)
    {
        data &= ~0x3F;
        data |= (from & 0x3F);
    }

    inline void set_to(uint32_t to)
    {
        data &= ~(0x3F << 6);
        data |= (to & 0x3F) << 6;
    }

    inline void set_color(COLORS color)
    {
        data &= ~(1 << 12);
        data |= (uint32_t(color) & 1) << 12;
    }

    inline void set_piece(PIECE_TYPE piece)
    {
        data &= ~(0x7 << 13);
        data |= (uint32_t(piece) & 0x7) << 13;
    }

    inline void set_captured(PIECE_TYPE captured)
    {
        data &= ~(0x7 << 16);
        data |= (uint32_t(captured) & 0x7) << 16;
    }

    inline void set_promotion(PIECE_TYPE promotion)
    {
        data &= ~(0x7 << 19);
        data |= (uint32_t(promotion) & 0x7) << 19;
    }

    inline void set_castle_rights(uint8_t castle_rights)
    {
        data &= ~(0xF << 22);
        data |= (uint32_t(castle_rights) & 0xF) << 22;
    }

    inline void set_type(MOVE_TYPE type)
    {
        data &= ~(0xF << 26);
        data |= (uint32_t(type) & 0xF) << 26;
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
        return static_cast<MOVE_TYPE>(get_bites(data, 26, 29));
    }
};