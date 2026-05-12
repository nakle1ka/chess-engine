#include "./rays.hpp"
#include "../shared/constants.hpp"

Rays::Rays()
{
    const bitboard ONE = 1ULL;

    for (int sq = 0; sq < 64; sq++)
    {
        bitboard b = ONE << sq;

        rays[(int)DIRECTION::NORTH][sq] = (FILE_A & ~RANK_1) << sq;
        rays[(int)DIRECTION::SOUTH][sq] = (FILE_H & ~RANK_8) >> (63 - sq);
        rays[(int)DIRECTION::WEST][sq] = (ONE << sq) - (ONE << (sq & 56));
        rays[(int)DIRECTION::EAST][sq] = 2 * ((ONE << (sq | 7)) - (ONE << sq));

        rays[(int)DIRECTION::NORTH_EAST][sq] = 0;
        for (bitboard temp = (b << 9) & ~FILE_A; temp != 0; temp = (temp << 9) & ~FILE_A)
            rays[(int)DIRECTION::NORTH_EAST][sq] |= temp;

        rays[(int)DIRECTION::NORTH_WEST][sq] = 0;
        for (bitboard temp = (b << 7) & ~FILE_H; temp != 0; temp = (temp << 7) & ~FILE_H)
            rays[(int)DIRECTION::NORTH_WEST][sq] |= temp;

        rays[(int)DIRECTION::SOUTH_EAST][sq] = 0;
        for (bitboard temp = (b >> 7) & ~FILE_A; temp != 0; temp = (temp >> 7) & ~FILE_A)
            rays[(int)DIRECTION::SOUTH_EAST][sq] |= temp;

        rays[(int)DIRECTION::SOUTH_WEST][sq] = 0;
        for (bitboard temp = (b >> 9) & ~FILE_H; temp != 0; temp = (temp >> 9) & ~FILE_H)
            rays[(int)DIRECTION::SOUTH_WEST][sq] |= temp;
    }
}

bitboard Rays::get_ray(DIRECTION dir, int sq) const
{
    return rays[(int)dir][sq];
}