#pragma once
#include "../rays/rays.hpp"

class Attacks
{
private:
    Rays *rays;

public:
    Attacks(Rays *_rays);

    bitboard get_pawn_advances(COLORS color, bitboard bb);
    bitboard get_pawn_attacks(COLORS color, bitboard bb);
    bitboard get_knights_attacks(bitboard bb);
    bitboard get_king_attacks(bitboard bb);

    bitboard get_rook_attacks(int sq, bitboard blockers);
    bitboard get_bishop_attacks(int sq, bitboard blockers);
    bitboard get_queen_attacks(int sq, bitboard blockers);

private:
    bitboard get_ray_attacks(DIRECTION dir, int sq, bitboard blockers, bool use_bsf);
};
