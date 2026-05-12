#include <memory>
#include "./attacks.hpp"
#include "../shared/utils.hpp"
#include "../shared/types.hpp"
#include "../shared/constants.hpp"

Attacks::Attacks(Rays *_rays) : rays(_rays) {}

bitboard Attacks::get_pawn_advances(COLORS color, bitboard bb)
{
    switch (color)
    {
    case COLORS::WHITE:
        return (bb << 8) | (bb & RANK_2) << 16;
    case COLORS::BLACK:
        return (bb >> 8) | (bb & RANK_7) >> 16;
    }
    return 0;
}

bitboard Attacks::get_pawn_attacks(COLORS color, bitboard bb)
{
    switch (color)
    {
    case COLORS::WHITE:
        return ((bb << 9) & (~FILE_A)) | ((bb << 7) & (~FILE_H));
    case COLORS::BLACK:
        return ((bb >> 9) & (~FILE_H)) | ((bb >> 7) & (~FILE_A));
    }
    return 0;
}

bitboard Attacks::get_knights_attacks(bitboard bb)
{

    bitboard l1 = (bb >> 1) & (~FILE_H);
    bitboard l2 = (bb >> 2) & ~(FILE_H | FILE_G);
    bitboard r1 = (bb << 1) & (~FILE_A);
    bitboard r2 = (bb << 2) & ~(FILE_A | FILE_B);

    return (l1 | r1) << 16 | (l1 | r1) >> 16 | (l2 | r2) << 8 | (l2 | r2) >> 8;
}

bitboard Attacks::get_king_attacks(bitboard bb)
{
    bitboard attacks = 0;

    attacks |= (bb << 1) & ~FILE_A;
    attacks |= (bb >> 1) & ~FILE_H;

    attacks |= (bb << 8);
    attacks |= (bb >> 8);

    attacks |= (bb << 9) & ~FILE_A;
    attacks |= (bb >> 7) & ~FILE_A;
    attacks |= (bb << 7) & ~FILE_H;
    attacks |= (bb >> 9) & ~FILE_H;

    return attacks;
}

bitboard Attacks::get_rook_attacks(int sq, bitboard blockers)
{
    return get_ray_attacks(DIRECTION::NORTH, sq, blockers, true) |
           get_ray_attacks(DIRECTION::EAST, sq, blockers, true) |
           get_ray_attacks(DIRECTION::SOUTH, sq, blockers, false) |
           get_ray_attacks(DIRECTION::WEST, sq, blockers, false);
}

bitboard Attacks::get_bishop_attacks(int sq, bitboard blockers)
{
    return get_ray_attacks(DIRECTION::NORTH_EAST, sq, blockers, true) |
           get_ray_attacks(DIRECTION::NORTH_WEST, sq, blockers, true) |
           get_ray_attacks(DIRECTION::SOUTH_EAST, sq, blockers, false) |
           get_ray_attacks(DIRECTION::SOUTH_WEST, sq, blockers, false);
}

bitboard Attacks::get_queen_attacks(int sq, bitboard blockers)
{
    return get_rook_attacks(sq, blockers) | get_bishop_attacks(sq, blockers);
}

bitboard Attacks::get_ray_attacks(DIRECTION dir, int sq, bitboard blockers, bool use_bsf)
{
    bitboard ray = rays->get_ray(dir, sq);
    bitboard mask = blockers & ray;
    if (!mask)
        return ray;
    int blocker_sq = use_bsf ? bitScanForward(mask) : bitScanReverse(mask);
    return ray & ~rays->get_ray(dir, blocker_sq);
}